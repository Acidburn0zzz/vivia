/*ckwg +5
 * Copyright 2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include "vsTrackOracleTrackArchiveSource.h"

#include <QDebug>
#include <QSet>

#include <qtThread.h>
#include <qtStlUtil.h>

#include <vgGeodesy.h>

#include <track_oracle/file_format_base.h>

#include <vsTrackData.h>

#include <vsAdapt.h>
#include <vsArchiveSourcePrivate.h>

#include "visgui_track_type.h"

//-----------------------------------------------------------------------------
class vsTrackOracleTrackArchiveSourcePrivate : public vsArchiveSourcePrivate
{
public:
  typedef vidtk::file_format_base FileFormat;

  vsTrackOracleTrackArchiveSourcePrivate(
    vsTrackOracleTrackArchiveSource* q, const QUrl& uri, FileFormat* format);

protected:
  QTE_DECLARE_PUBLIC(vsTrackOracleTrackArchiveSource)

  virtual bool processArchive(const QUrl& uri) QTE_OVERRIDE;

  FileFormat* Format;
};

QTE_IMPLEMENT_D_FUNC(vsTrackOracleTrackArchiveSource)

//-----------------------------------------------------------------------------
vsTrackOracleTrackArchiveSourcePrivate::vsTrackOracleTrackArchiveSourcePrivate(
  vsTrackOracleTrackArchiveSource* q, const QUrl& uri, FileFormat* format)
  : vsArchiveSourcePrivate(q, "tracks", uri), Format(format)
{
}

//-----------------------------------------------------------------------------
bool vsTrackOracleTrackArchiveSourcePrivate::processArchive(const QUrl& uri)
{
  QTE_Q(vsTrackOracleTrackArchiveSource);

  const std::string fileName = stdString(uri.toLocalFile());

  // Read tracks
  const visgui_track_type schema;
  vidtk::track_handle_list_type tracks;
  std::vector<vidtk::element_descriptor> missingFieldDescriptors;

  if (!this->Format->read(fileName, tracks, schema, missingFieldDescriptors))
    {
    qWarning() << "error reading tracks from" << uri;
    return false;
    }

  // Are we missing any fields?
  if (missingFieldDescriptors.size() > 0)
    {
    // Build set of missing fields
    QSet<QString> missingFields;
    for (size_t n = 0, k = missingFieldDescriptors.size(); n < k; ++n)
      {
      missingFields.insert(qtString(missingFieldDescriptors[n].name));
      }

    // Check if missing field(s) is/are mandatory; we are okay without frame
    // number
    // TODO Allow missing time value once core supports time mapping
    if (missingFields.count() > 1 ||
        *missingFields.begin() != "frame_number")
      {
      qWarning() << "unable to load tracks from" << uri
                 << "due to missing fields" << missingFields;
      return false;
      }
    }

  // Process the tracks
  visgui_track_type oracle;
  for (size_t n = 0, k = tracks.size(); n < k; ++n)
    {
    const vidtk::track_handle_type& trackHandle = tracks[n];
    if (trackHandle.is_valid())
      {
      // Set oracle object to track instance referenced by handle
      oracle(trackHandle);

      // Get track ID
      vsTrackId id = vsAdaptTrackId(oracle.external_id());

      // Convert track states
      QList<vvTrackState> states;
      vsTrackData data;
      vidtk::frame_handle_list_type frameHandles =
        vidtk::track_oracle::get_frames(trackHandle);
      for (size_t n = 0, k = frameHandles.size(); n < k; ++n)
        {
        const vidtk::frame_handle_type& frameHandle = frameHandles[n];
        if (frameHandle.is_valid())
          {
          vvTrackState state;

          // Set oracle object to frame instance referenced by handle
          oracle[frameHandle];

          // Extract timestamp
          if (oracle.timestamp_usecs.exists())
            {
            state.TimeStamp.Time = oracle.timestamp_usecs();
            }
          if (oracle.frame_number.exists())
            {
            state.TimeStamp.FrameNumber = oracle.frame_number();
            }
          if (!state.TimeStamp.IsValid())
            {
            // Don't add state if timestamp is not valid
            continue;
            }

          // Extract bounding box
          const vgl_box_2d<double>& vglBox = oracle.bounding_box();
          vvImageBoundingBox& box = state.ImageBox;
          box.TopLeft.X = vglBox.min_x();
          box.TopLeft.Y = vglBox.min_y();
          box.BottomRight.X = vglBox.max_x();
          box.BottomRight.Y = vglBox.max_y();

          // Copy bounding box to object
          state.ImageObject.push_back(
            vvImagePointF(box.TopLeft.X, box.TopLeft.Y));
          state.ImageObject.push_back(
            vvImagePointF(box.BottomRight.X, box.TopLeft.Y));
          state.ImageObject.push_back(
            vvImagePointF(box.BottomRight.X, box.BottomRight.Y));
          state.ImageObject.push_back(
            vvImagePointF(box.TopLeft.X, box.BottomRight.Y));

          // Extract image location
          const vgl_point_2d<double>& vglPoint = oracle.obj_location();
          state.ImagePoint.X = vglPoint.x();
          state.ImagePoint.Y = vglPoint.y();

          // Add state
          states.append(state);
          }
        }

      // Emit track (but skip empty tracks)
      if (!states.isEmpty())
        {
        emit q->trackUpdated(id, states);
        emit q->trackDataUpdated(id, data);
        emit q->trackClosed(id);
        }
      }
    }

  // Done
  return true;
}

//-----------------------------------------------------------------------------
vsTrackOracleTrackArchiveSource::vsTrackOracleTrackArchiveSource(
  const QUrl& uri, vidtk::file_format_base* format) :
  super(new vsTrackOracleTrackArchiveSourcePrivate(this, uri, format))
{
}

//-----------------------------------------------------------------------------
vsTrackOracleTrackArchiveSource::~vsTrackOracleTrackArchiveSource()
{
}

/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   QuickTime video output module

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "common/common_pch.h"

#include "common/codec.h"
#include "output/p_quicktime.h"

quicktime_video_packetizer_c::quicktime_video_packetizer_c(generic_reader_c *p_reader,
                                                           track_info_c &p_ti,
                                                           int width,
                                                           int height)
  : video_packetizer_c{p_reader, p_ti, MKV_V_QUICKTIME, 0.0, width, height}
{
}

connection_result_e
quicktime_video_packetizer_c::can_connect_to(generic_packetizer_c *src,
                                             std::string &error_message) {
  auto qvsrc = dynamic_cast<quicktime_video_packetizer_c *>(src);
  if (!qvsrc)
    return CAN_CONNECT_NO_FORMAT;

  return video_packetizer_c::can_connect_to(src, error_message);
}

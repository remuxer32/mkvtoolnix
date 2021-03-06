/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   the track info params class

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "common/common_pch.h"

#include <matroska/KaxTags.h>

#include "common/ebml.h"
#include "merge/track_info.h"

track_info_c::track_info_c()
  : m_id{}
  , m_disable_multi_file{}
  , m_aspect_ratio{}
  , m_display_width{}
  , m_display_height{}
  , m_aspect_ratio_given{}
  , m_aspect_ratio_is_factor{}
  , m_display_dimensions_given{}
  , m_display_dimensions_source{OPTION_SOURCE_NONE}
  , m_reset_timecodes{}
  , m_cues{CUE_STRATEGY_UNSPECIFIED}
  , m_default_track{boost::logic::indeterminate}
  , m_fix_bitstream_frame_rate{boost::logic::indeterminate}
  , m_forced_track{boost::logic::indeterminate}
  , m_enabled_track{boost::logic::indeterminate}
  , m_compression{COMPRESSION_UNSPECIFIED}
  , m_nalu_size_length{}
  , m_no_chapters{}
  , m_no_global_tags{}
  , m_avi_audio_sync_enabled{}
{
}

track_info_c &
track_info_c::operator =(const track_info_c &src) {
  m_id                         = src.m_id;
  m_fname                      = src.m_fname;

  m_atracks                    = src.m_atracks;
  m_btracks                    = src.m_btracks;
  m_stracks                    = src.m_stracks;
  m_vtracks                    = src.m_vtracks;
  m_track_tags                 = src.m_track_tags;
  m_disable_multi_file         = src.m_disable_multi_file;

  m_private_data               = src.m_private_data ? src.m_private_data->clone() : src.m_private_data;

  m_all_fourccs                = src.m_all_fourccs;

  m_display_properties         = src.m_display_properties;
  m_aspect_ratio               = src.m_aspect_ratio;
  m_aspect_ratio_given         = false;
  m_aspect_ratio_is_factor     = false;
  m_display_dimensions_given   = false;
  m_display_dimensions_source  = src.m_display_dimensions_source;

  m_timecode_syncs             = src.m_timecode_syncs;
  m_tcsync                     = src.m_tcsync;

  m_reset_timecodes_specs      = src.m_reset_timecodes_specs;
  m_reset_timecodes            = src.m_reset_timecodes;

  m_cue_creations              = src.m_cue_creations;
  m_cues                       = src.m_cues;

  m_default_track_flags        = src.m_default_track_flags;
  m_default_track              = src.m_default_track;

  m_fix_bitstream_frame_rate_flags = src.m_fix_bitstream_frame_rate_flags;
  m_fix_bitstream_frame_rate       = src.m_fix_bitstream_frame_rate;

  m_forced_track_flags         = src.m_forced_track_flags;
  m_forced_track               = src.m_forced_track;

  m_reduce_to_core             = src.m_reduce_to_core;

  m_enabled_track_flags        = src.m_enabled_track_flags;
  m_enabled_track              = src.m_enabled_track;

  m_languages                  = src.m_languages;
  m_language                   = src.m_language;

  m_sub_charsets               = src.m_sub_charsets;
  m_sub_charset                = src.m_sub_charset;

  m_all_tags                   = src.m_all_tags;
  m_tags_file_name             = src.m_tags_file_name;
  m_tags                       = src.m_tags ? clone(src.m_tags) : std::shared_ptr<KaxTags>{};

  m_all_aac_is_sbr             = src.m_all_aac_is_sbr;

  m_compression_list           = src.m_compression_list;
  m_compression                = src.m_compression;

  m_track_names                = src.m_track_names;
  m_track_name                 = src.m_track_name;

  m_all_ext_timecodes          = src.m_all_ext_timecodes;
  m_ext_timecodes              = src.m_ext_timecodes;

  m_pixel_crop_list            = src.m_pixel_crop_list;
  m_pixel_cropping             = src.m_pixel_cropping;

  m_stereo_mode_list           = src.m_stereo_mode_list;
  m_stereo_mode                = src.m_stereo_mode;

  m_nalu_size_lengths          = src.m_nalu_size_lengths;
  m_nalu_size_length           = src.m_nalu_size_length;

  m_attach_mode_list           = src.m_attach_mode_list;

  m_no_chapters                = src.m_no_chapters;
  m_no_global_tags             = src.m_no_global_tags;

  m_chapter_charset            = src.m_chapter_charset;
  m_chapter_language           = src.m_chapter_language;

  m_avi_audio_sync_enabled     = false;
  m_default_durations          = src.m_default_durations;
  m_max_blockadd_ids           = src.m_max_blockadd_ids;

  return *this;
}

bool
track_info_c::display_dimensions_or_aspect_ratio_set() {
  return OPTION_SOURCE_NONE != m_display_dimensions_source;
}

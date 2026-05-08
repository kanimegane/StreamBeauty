#include "beauty-filter.h"
#include <obs-module.h>
#include <plugin-support.h>
#include <graphics/graphics.h>
#include <graphics/image-file.h>
#include <util/platform.h>

#define SETTING_SMOOTHNESS  "smoothness"
#define SETTING_BRIGHTNESS  "brightness"
#define SETTING_BLEND       "blend"
#define SETTING_SATURATION  "saturation"
#define SETTING_COMPARE     "compare"

#define TEXT_SMOOTHNESS  "Smoothness"
#define TEXT_BRIGHTNESS  "Brightness"
#define TEXT_BLEND       "Blend"
#define TEXT_SATURATION  "Saturation"
#define TEXT_COMPARE     "Before/After Compare"

struct beauty_filter_data {
	obs_source_t *context;
	gs_effect_t  *effect;

	gs_eparam_t  *param_smoothness;
	gs_eparam_t  *param_brightness;
	gs_eparam_t  *param_blend;
	gs_eparam_t  *param_saturation;
	gs_eparam_t  *param_compare;

	float smoothness;
	float brightness;
	float blend;
	float saturation;
	bool  compare;
};

static const char *beauty_filter_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "StreamBeauty Skin Filter";
}

static void beauty_filter_update(void *data, obs_data_t *settings)
{
	struct beauty_filter_data *filter = data;
	filter->smoothness = (float)obs_data_get_double(settings, SETTING_SMOOTHNESS);
	filter->brightness = (float)obs_data_get_double(settings, SETTING_BRIGHTNESS);
	filter->blend      = (float)obs_data_get_double(settings, SETTING_BLEND);
	filter->saturation = (float)obs_data_get_double(settings, SETTING_SATURATION);
	filter->compare    = obs_data_get_bool(settings, SETTING_COMPARE);
}

static void *beauty_filter_create(obs_data_t *settings, obs_source_t *context)
{
	struct beauty_filter_data *filter = bzalloc(sizeof(*filter));
	filter->context = context;

	char *effect_path = obs_module_file("effects/beauty.effect");

	obs_enter_graphics();
	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	obs_leave_graphics();

	bfree(effect_path);

	if (!filter->effect) {
		obs_log(LOG_ERROR, "Failed to load beauty.effect");
		bfree(filter);
		return NULL;
	}

	filter->param_smoothness = gs_effect_get_param_by_name(filter->effect, "smoothness");
	filter->param_brightness = gs_effect_get_param_by_name(filter->effect, "brightness");
	filter->param_blend      = gs_effect_get_param_by_name(filter->effect, "blend");
	filter->param_saturation = gs_effect_get_param_by_name(filter->effect, "saturation");
	filter->param_compare    = gs_effect_get_param_by_name(filter->effect, "compare_pos");

	beauty_filter_update(filter, settings);
	return filter;
}

static void beauty_filter_destroy(void *data)
{
	struct beauty_filter_data *filter = data;
	obs_enter_graphics();
	gs_effect_destroy(filter->effect);
	obs_leave_graphics();
	bfree(filter);
}

static obs_properties_t *beauty_filter_properties(void *unused)
{
	UNUSED_PARAMETER(unused);
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_float_slider(props, SETTING_SMOOTHNESS, TEXT_SMOOTHNESS, 0.0, 1.0, 0.01);
	obs_properties_add_float_slider(props, SETTING_BRIGHTNESS, TEXT_BRIGHTNESS, -0.5, 0.5, 0.01);
	obs_properties_add_float_slider(props, SETTING_BLEND,      TEXT_BLEND,      0.0, 1.0, 0.01);
	obs_properties_add_float_slider(props, SETTING_SATURATION, TEXT_SATURATION, 0.0, 2.0, 0.01);
	obs_properties_add_bool(props, SETTING_COMPARE, TEXT_COMPARE);

	return props;
}

static void beauty_filter_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, SETTING_SMOOTHNESS, 0.6);
	obs_data_set_default_double(settings, SETTING_BRIGHTNESS, 0.05);
	obs_data_set_default_double(settings, SETTING_BLEND,      0.8);
	obs_data_set_default_double(settings, SETTING_SATURATION, 1.1);
	obs_data_set_default_bool(settings,   SETTING_COMPARE,    false);
}

static void beauty_filter_render(void *data, gs_effect_t *unused)
{
	UNUSED_PARAMETER(unused);
	struct beauty_filter_data *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING))
		return;

	gs_effect_set_float(filter->param_smoothness, filter->smoothness);
	gs_effect_set_float(filter->param_brightness, filter->brightness);
	gs_effect_set_float(filter->param_blend,      filter->blend);
	gs_effect_set_float(filter->param_saturation, filter->saturation);
	gs_effect_set_float(filter->param_compare,    filter->compare ? 0.5f : 1.0f);

	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
}

void register_beauty_filter(void)
{
	struct obs_source_info info = {
		.id             = "stream_beauty_filter",
		.type           = OBS_SOURCE_TYPE_FILTER,
		.output_flags   = OBS_SOURCE_VIDEO,
		.get_name       = beauty_filter_name,
		.create         = beauty_filter_create,
		.destroy        = beauty_filter_destroy,
		.update         = beauty_filter_update,
		.get_properties = beauty_filter_properties,
		.get_defaults   = beauty_filter_defaults,
		.video_render   = beauty_filter_render,
	};
	obs_register_source(&info);
}

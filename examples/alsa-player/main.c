#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

int main() {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = 44100;
    int dir;
    int rc;

    printf("ALSA Cross-Compilation Demo\n");
    printf("ALSA library version: %s\n", SND_LIB_VERSION_STR);

    // 打开默认音频设备
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "无法打开音频设备: %s\n", snd_strerror(rc));
        return 1;
    }

    printf("音频设备打开成功!\n");

    // 分配硬件参数空间
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);

    // 设置参数
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);

    // 应用参数
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "无法设置硬件参数: %s\n", snd_strerror(rc));
        snd_pcm_close(handle);
        return 1;
    }

    printf("采样率: %d Hz\n", rate);
    printf("声道数: 2\n");
    printf("格式: 16-bit little-endian\n");

    // 关闭设备
    snd_pcm_close(handle);
    printf("设备关闭成功\n");

    return 0;
}

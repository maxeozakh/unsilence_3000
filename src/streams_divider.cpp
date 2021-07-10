#include <iostream>
#include <string>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <inttypes.h>
}

const char* video_file_path = "/Users/mxeo/Magic/unsilence_3000/assets/clip.mp4";

bool load_frame(const char* filename) {
    std::cout << filename << '\n';
    printf("\n");


    // open input (open file based on filename and fill and place info about them into av_format_ctx)

    AVFormatContext* av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        printf("Couldn't created AVFormatContext\n");
    }
    
    if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) != 0) {
        printf("Couldn't open video file\n");
        return false;
    }

    // try to find video & audio streams

    int video_stream_index = -1;
    int audio_stream_index = -1;

    AVStream* stream;
    AVCodecParameters* av_codec_params;
    AVCodecID av_codec_id;
    AVCodec* av_codec;

    for (int i = 0; i < av_format_ctx -> nb_streams; i++) {
        stream = av_format_ctx -> streams[i];
        av_codec_params = stream -> codecpar;
        av_codec_id = av_codec_params -> codec_id;
        av_codec = avcodec_find_decoder(av_codec_id);

        if (!av_codec) {
            std::cout << "Couldn't find codec for stream number " << i << '\n';
        }

        if (av_codec_params -> codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        }

        if (av_codec_params -> codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
        }
    }

    if (video_stream_index == -1 || audio_stream_index == -1) {
        std::cout << "Couldn't find some stream index\n";
    }

    return false;
}

void streams_divider() {
    std::cout << "STREAMS_DIVIDER invoked" << '\n';
    if (!load_frame(video_file_path)) {
        printf("Couldn't open video frame\n");
    }
    
}
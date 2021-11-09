#include <iostream>
#include <string>
#include "streams_divider.hpp"

bool error_handler(const char *error_message);

static const char *av_make_error(int errnum)
{
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

const char *video_file_path = "/Users/mxeo/Magic/unsilence_3000/assets/clip.mp4";

bool open_input(AVFormatContext &format_context, const char *filename)
{
    format_context = *avformat_alloc_context();
    if (!&format_context)
    {
        printf("Couldn't created AVFormatContext\n");
        return false;
    }

    // // open file based on filename and fill and place info
    // // about them into formatContext

    AVFormatContext *ptr = &format_context;
    AVFormatContext **ptr_to_ptr = &ptr;

    if (avformat_open_input(ptr_to_ptr, filename, NULL, NULL) != 0)
    {
        error_handler("Couldn't open video file");
        return false;
    }

    return true;
}

void find_video_stream(AVFormatContext &format_context, AVCodecParameters &codec_params, AVStream &video_stream, int &video_stream_index)
{
    video_stream_index = -1;

    AVStream *local_stream;
    AVCodecParameters *local_codec_params;

    for (int i = 0; i < format_context.nb_streams; i++)
    {
        local_stream = format_context.streams[i];
        local_codec_params = local_stream->codecpar;

        if (local_codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            codec_params = *local_codec_params;
            video_stream = *local_stream;
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1)
    {
        std::cout << "Couldn't find video stream \n";
    }
}

void find_codec_for_stream(AVCodecParameters &video_codec_params, AVCodec &video_codec)
{
    AVCodecID av_codec_id = video_codec_params.codec_id;
    video_codec = *avcodec_find_decoder(av_codec_id);

    if (!&video_codec)
    {
        std::cout << "Couldn't find codec for video stream" << std::endl;
    }
}

void setup_codec_context(AVCodec &video_codec, AVCodecContext &video_codec_context, AVCodecParameters &video_codec_params)
{
    video_codec_context = *avcodec_alloc_context3(&video_codec);
    if (!&video_codec_context)
    {
        error_handler("Couldn't create AVCodecContext");
    }

    // fill codec context with sort of init state
    if (avcodec_parameters_to_context(&video_codec_context, &video_codec_params) < 0)
    {
        error_handler("Couldn't initialize AVCodecContext");
    }

    if (avcodec_open2(&video_codec_context, &video_codec, NULL) < 0)
    {
        error_handler("Couldn't open codec");
    }
}

bool load_frame(AVFormatContext &format_context, AVCodecContext &video_codec_context, int &video_stream_index);

void streams_divider()
{
    std::cout << "STREAMS_DIVIDER invoked" << '\n';
    AVFormatContext format_context;
    AVStream video_stream;
    AVCodecParameters video_codec_params;
    AVCodec video_codec;
    AVCodecContext video_codec_context;
    int video_stream_index;

    open_input(format_context, video_file_path);
    find_video_stream(format_context, video_codec_params, video_stream, video_stream_index);

    find_codec_for_stream(video_codec_params, video_codec);
    int debugger = 0;
    setup_codec_context(video_codec, video_codec_context, video_codec_params);

    if (!load_frame(format_context, video_codec_context, video_stream_index))
    {
        printf("Couldn't open video frame\n");
    }
}

bool load_frame(AVFormatContext &format_context, AVCodecContext &video_codec_context, int &video_stream_index)
{
    AVFrame *av_frame = av_frame_alloc();
    if (!av_frame)
    {
        error_handler("Couldn't allocate AVFrame");
    }
    AVPacket *av_packet = av_packet_alloc();
    if (!av_packet)
    {
        error_handler("Couldn't allocate AVPacket");
    }

    int response;
    // its read packet, not a frame
    int frame_counter;
    while (av_read_frame(&format_context, av_packet) >= 0)
    {
        if (av_packet->stream_index != video_stream_index)
        {
            av_packet_unref(av_packet);
            continue;
        }

        response = avcodec_send_packet(&video_codec_context, av_packet);
        if (response < 0)
        {
            const char *error = av_make_error(response);
            error_handler(error);
            return false;
        }

        response = avcodec_receive_frame(&video_codec_context, av_frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
            std::cout << "AVERROR: EAGAIN OR EOF, response is: " << response << "\n";
            continue;
        }
        else if (response < 0)
        {
            error_handler("Failed to decode packet");
        }
        frame_counter++;
        std::cout << "OUR RESPONSE BOYZ: " << response << "\n";
    }

    std::cout << "frame count is " << frame_counter / 24 << '\n';

    // // desctructors - cleanup everything
    // avformat_close_input(format_context);
    // avformat_free_context(format_context);
    // avcodec_free_context(video_codec_context);

    return false;
}
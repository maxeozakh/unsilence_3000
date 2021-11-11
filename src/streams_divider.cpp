#include <iostream>
#include <string>
#include "streams_divider.hpp"

const char *video_file_path = "/Users/mxeo/Magic/unsilence_3000/assets/clip.mp4";

bool error_handler(const char *error_message);
static const char *av_make_error(int errnum);
bool error_handler(const char *error_message);
static const char *av_make_error(int errnum);
bool open_input(const char *filename, AVFormatContext &out_format_context);
void get_codec_parameters(AVFormatContext &format_context,
                          AVMediaType media_type,
                          AVCodecParameters &out_codec_params);
void get_stream_and_stream_index(AVFormatContext &format_context,
                                 AVMediaType media_type,
                                 AVStream &out_stream,
                                 int &out_stream_index);
void get_codec(AVCodecID &codec_id, AVCodec &out_codec);
void setup_codec_context(AVCodec &codec, AVCodecParameters &codec_params,
                         AVCodecContext &out_codec_context);
bool read_frame(AVFormatContext &format_context,
                AVCodecContext &video_codec_context,
                int &video_stream_index,
                AVFrame frames[]);
void find_video_stream(AVFormatContext &format_context,
                       AVCodecParameters &codec_params,
                       AVStream &video_stream,
                       int &video_stream_index);

void streams_divider()
{
    std::cout << "STREAMS_DIVIDER invoked" << '\n';
    AVFrame frames[10000];

    AVFormatContext format_context;
    open_input(video_file_path, format_context);

    AVCodecParameters video_codec_params;
    get_codec_parameters(format_context, AVMEDIA_TYPE_VIDEO, video_codec_params);

    AVStream video_stream;
    int video_stream_index = -1;
    get_stream_and_stream_index(format_context, AVMEDIA_TYPE_VIDEO, video_stream, video_stream_index);

    AVCodecID video_codec_id = video_codec_params.codec_id;
    AVCodec video_codec;
    get_codec(video_codec_id, video_codec);

    AVCodecContext video_codec_context;
    setup_codec_context(video_codec, video_codec_params, video_codec_context);

    if (!read_frame(format_context, video_codec_context, video_stream_index, frames))
    {
        printf("Couldn't open video frame\n");
    }
}

bool read_frame(AVFormatContext &format_context,
                AVCodecContext &video_codec_context,
                int &video_stream_index,
                AVFrame frames[])
{
    AVPacket *av_packet = av_packet_alloc();
    if (!av_packet)
    {
        error_handler("Couldn't allocate AVPacket");
    }

    AVFrame *av_frame = av_frame_alloc();
    if (!av_frame)
    {
        error_handler("Couldn't allocate AVFrame");
    }

    int response;
    // its read packet, not a frame
    int frames_counter = 0;
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
        frames[frames_counter] = *av_frame;
        frames_counter++;
    }

    std::cout << "frame count is " << frames_counter << '\n';
    // for (int i = 0; i < frames_counter; i++) {
    //     std::cout << frames[i].metadata << '\n';
    // }

    // // desctructors - cleanup everything
    // avformat_close_input(format_context);
    // avformat_free_context(format_context);
    // avcodec_free_context(video_codec_context);

    return true;
}

bool open_input(const char *filename, AVFormatContext &out_format_context)
{
    AVFormatContext *format_context_pointer = avformat_alloc_context();
    if (!format_context_pointer)
    {
        printf("Couldn't created AVFormatContext\n");
        return false;
    }
    out_format_context = *format_context_pointer;

    // open file based on filename and fill and place info
    // about them into formatContext

    AVFormatContext *ptr = &out_format_context;
    AVFormatContext **ptr_to_ptr = &ptr;

    if (avformat_open_input(ptr_to_ptr, filename, NULL, NULL) != 0)
    {
        error_handler("Couldn't open video file");
        return false;
    }

    return true;
}

void get_codec_parameters(AVFormatContext &format_context,
                          AVMediaType media_type,
                          AVCodecParameters &out_codec_params)
{
    AVStream *local_stream;
    AVCodecParameters *local_codec_params;

    for (int i = 0; i < format_context.nb_streams; i++)
    {
        local_stream = format_context.streams[i];
        local_codec_params = local_stream->codecpar;

        if (local_codec_params->codec_type == media_type)
        {
            out_codec_params = *local_codec_params;
        }
    }
}

void get_stream_and_stream_index(AVFormatContext &format_context,
                                 AVMediaType media_type,
                                 AVStream &out_stream,
                                 int &out_stream_index)
{
    AVStream *local_stream;
    AVCodecParameters *local_codec_params;

    for (int i = 0; i < format_context.nb_streams; i++)
    {
        local_stream = format_context.streams[i];
        local_codec_params = local_stream->codecpar;

        if (local_codec_params->codec_type == media_type)
        {
            out_stream = *local_stream;
            out_stream_index = i;
            break;
        }
    }

    if (out_stream_index == -1)
    {
        std::cout << "Couldn't find video stream \n";
    }
}

void find_video_stream(AVFormatContext &format_context,
                       AVCodecParameters &codec_params,
                       AVStream &video_stream,
                       int &video_stream_index)
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
void get_codec(AVCodecID &codec_id, AVCodec &out_codec)
{
    AVCodec *codec_ref = avcodec_find_decoder(codec_id);

    if (!codec_ref)
    {
        std::cout << "Couldn't find codec for video stream" << std::endl;
        return;
    }

    out_codec = *codec_ref;
}

void setup_codec_context(AVCodec &codec,
                         AVCodecParameters &codec_params,
                         AVCodecContext &out_codec_context)
{
    AVCodecContext *codec_context_pointer = avcodec_alloc_context3(&codec);
    if (!codec_context_pointer)
    {
        error_handler("Couldn't create AVCodecContext");
        return;
    }
    out_codec_context = *codec_context_pointer;

    if (avcodec_parameters_to_context(&out_codec_context, &codec_params) < 0)
    {
        error_handler("Couldn't initialize AVCodecContext");
    }

    if (avcodec_open2(&out_codec_context, &codec, NULL) < 0)
    {
        error_handler("Couldn't open codec");
    }
}

static const char *av_make_error(int errnum)
{
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

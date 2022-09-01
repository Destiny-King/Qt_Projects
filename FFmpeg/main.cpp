#include <stdio.h>

#pragma warning(disable : 4996)

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
}

// FIX
struct URLProtocol;

/**
 * Protocol Support Information
 */
char *urlprotocolinfo() {
  char *info = (char *)malloc(40000);
  memset(info, 0, 40000);

  struct URLProtocol *pup = NULL;
  // input
  struct URLProtocol **p_temp = &pup;
  avio_enum_protocols((void **)p_temp, 0);
  while ((*p_temp) != NULL) {
    sprintf(info, "%s[In][%10s]\n", info,
            avio_enum_protocols((void **)p_temp, 0));
  }
  pup = NULL;
  // output
  avio_enum_protocols((void **)p_temp, 1);
  while ((*p_temp) != NULL) {
    sprintf(info, "%s[Out][%10s]\n", info,
            avio_enum_protocols((void **)p_temp, 1));
  }
  return info;
}

/**
 * AVFormat Support information
 */
char *avformatinfo() {
  char *info = (char *)malloc(40000);
  memset(info, 0, 40000);
  AVInputFormat *if_temp = av_iformat_next(NULL);
  AVOutputFormat *of_temp = av_oformat_next(NULL);
  // input
  while (if_temp != NULL) {
    sprintf(info, "%s[In] %10s\n", info, if_temp->name);
    if_temp = if_temp->next;
  }
  // output
  while (of_temp != NULL) {
    sprintf(info, "%s[In] %10s\n", info, of_temp->name);
    of_temp = of_temp->next;
  }
  return info;
}

/**
 * AVCodec Support Information
 */
char *avcodecinfo() {
  char *info = (char *)malloc(40000);
  memset(info, 0, 40000);

  AVCodec *c_temp = av_codec_next(NULL);
  while (c_temp != NULL) {
    if (c_temp->decode != NULL) {
      sprintf(info, "%s[Dec]", info);
    } else {
      sprintf(info, "%s[Enc]", info);
    }
    switch (c_temp->type) {
    case AVMEDIA_TYPE_VIDEO:
      sprintf(info, "%s[Video]", info);
      break;
    case AVMEDIA_TYPE_AUDIO:
      sprintf(info, "%s[Audio]", info);
      break;
    default:
      sprintf(info, "%s[Other]", info);
      break;
    }
    sprintf(info, "%s %10s\n", info, c_temp->name);
    c_temp = c_temp->next;
  }
  return info;
}

/**
 * AVFilter Support Information
 */
char *avfilterinfo() {
  char *info = (char *)malloc(40000);
  memset(info, 0, 40000);

  AVFilter *f_temp = (AVFilter *)avfilter_next(NULL);
  while (f_temp != NULL) {
    sprintf(info, "%s[%15s]\n", info, f_temp->name);
    f_temp = f_temp->next;
  }
  return info;
}

/*
 * Configuration Information
 */
char *configurationinfo() {
  char *info = (char *)malloc(40000);
  memset(info, 0, 40000);
  sprintf(info, "%s\n", avcodec_configuration());

  return info;
}

int main() {
  char *infostr = NULL;
  infostr = configurationinfo();
  printf("\n<<Configuration>>\n%s", infostr);
  free(infostr);

  infostr = urlprotocolinfo();
  printf("\n<<URLProtocol>>\n%s", infostr);
  free(infostr);

  infostr = avformatinfo();
  printf("\n<<AVFormat>>\n%s", infostr);
  free(infostr);

  infostr = avcodecinfo();
  printf("\n<<AVCodec>>\n%s", infostr);
  free(infostr);

  infostr = avfilterinfo();
  printf("\n<<AVFilter>>\n%s", infostr);
  free(infostr);
  return 0;
}
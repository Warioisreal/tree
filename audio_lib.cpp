#include <string.h>

#include "audio_lib.h"


void VoiceText(const char* src) {
    char audio_text[MAX_ANSWER_SIZE] = "say ";
    strncat(audio_text, src, MAX_ANSWER_SIZE);
    system(audio_text);
}

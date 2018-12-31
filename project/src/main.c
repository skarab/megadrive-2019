#include "sme.h"
#include "resources.h"
#include "scene_barrel.h"

int main(u16 hard)
{
    sme_Init(hard);
    SND_startPlay_PCM(music, sizeof(music), SOUND_RATE_16000, SOUND_PAN_CENTER, 1);    
    
	SCENE_Barrel();
	    
    sme_Exit();    
    return 0;
}

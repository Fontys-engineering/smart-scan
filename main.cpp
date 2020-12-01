#include "TrackStar.h"

int main()
{
    TrackStar obj1;
	obj1.TrackStar_init();
    obj1.TrackStar_config();
	obj1.TrackStar_attachsensor();
    obj1.TrackStar_readsensor();
    obj1.TrackStar_stoptransmit();


}
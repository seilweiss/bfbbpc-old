#ifndef ZMUSIC_H
#define ZMUSIC_H

#define eMusicSituation_Unk00 0

void zMusicInit();
void zMusicNotify(int situation);
void zMusicUpdate(float dt);
void zMusicKill();

#endif
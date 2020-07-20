#ifndef ZENTPLAYERANIMATIONTABLES_H
#define ZENTPLAYERANIMATIONTABLES_H

#include "print.h"

#define NEW_STATE(table, name, flags, userFlags)\
    xAnimTableNewState(table, name, flags, userFlags, 1.0f, NULL, NULL, 0.0f, NULL,\
                       NULL, xAnimDefaultBeforeEnter, NULL, NULL)

#define NEW_TRAN(table, source, dest, conditional, callback, flags, userFlags, priority,\
                 blendRecip)\
    xAnimTableNewTransition(table, source, dest, conditional, callback, flags,\
                            userFlags, 0.0f, 0.0f, priority, 0, blendRecip, NULL)

#define ADD_TRAN(table, tran, source) xAnimTableAddTransition(table, tran, source)

xAnimTable *zSandy_AnimTable()
{
    BFBBSTUB("zSandy_AnimTable");
    return NULL;
}

xAnimTable *zPatrick_AnimTable()
{
    BFBBSTUB("zPatrick_AnimTable");
    return NULL;
}

xAnimTable *zEntPlayer_AnimTable()
{
    static const char *HIT_STATES[] =
    {
        "Idle01",
        "SlipIdle01",
        "Walk01",
        "Run01",
        "Run02",
        "Run03",
        "SlipRun01",
        "Land01",
        "LandHigh01",
        "LandRun01",
        "Idle02",
        "Idle03",
        "Idle04",
        "Idle05",
        "Idle06",
        "Idle07",
        "Idle08",
        "Idle09",
        "Idle10",
        "Idle11",
        "Idle12",
        "Idle13",
        "Inactive01",
        "Inactive02",
        "Inactive03",
        "Inactive04",
        "Inactive05",
        "Inactive06",
        "Inactive07",
        "Inactive08",
        "Inactive09",
        "Inactive10",
        "TongueStart01",
        "TongueSlide01",
        "TongueJump01",
        "TongueJumpXtra01",
        "TongueDJumpApex01",
        "TongueLand01",
        "JumpStart01",
        "JumpApex01",
        "DJumpStart01",
        "DJumpLift01",
        "Fall01",
        "Bspin01",
        "BbashStart01",
        "BbashAttack01",
        "BbashStrike01",
        "BbounceStart01",
        "BbounceAttack01",
        "BbounceStrike01",
        "BounceStart01",
        "BounceLift01",
        "BounceApex01",
        "Bbowl01",
        "BbowlStart01",
        "BbowlWindup01",
        "BbowlToss01",
        "BbowlRecover01",
        "WallLaunch01",
        "WallFlight01",
        "WallFlight02",
        "WallLand01",
        "WallFall01",
        NULL
    };

    static const char *STANDARD_STATES[] =
    {
        "Idle01",
        "Walk01",
        "Run01",
        "Run02",
        "Run03",
        "RunOutOfWorld01",
        "SlipRun01",
        "SlipIdle01",
        "Land01",
        "LandHigh01",
        "Idle02",
        "Idle03",
        "Idle04",
        "Idle05",
        "Idle06",
        "Idle07",
        "Idle08",
        "Idle09",
        "Idle10",
        "Idle11",
        "Idle12",
        "Idle13",
        "Inactive01",
        "Inactive02",
        "Inactive03",
        "Inactive04",
        "Inactive05",
        "Inactive06",
        "Inactive07",
        "Inactive08",
        "Inactive09",
        "Inactive10",
        NULL
    };

    xAnimTransition *tran;
    xAnimTable *table;

    table = xAnimTableNew("SB", NULL, 0);

    NEW_STATE(table, "Idle01", 0x10, 0x1);
    NEW_STATE(table, "Idle02", 0x10, 0x1);
    NEW_STATE(table, "Idle03", 0x10, 0x1);
    NEW_STATE(table, "Idle04", 0x10, 0x1);
    NEW_STATE(table, "Idle05", 0x10, 0x1);
    NEW_STATE(table, "Idle06", 0x10, 0x1);
    NEW_STATE(table, "Idle07", 0x10, 0x1);
    NEW_STATE(table, "Idle08", 0x10, 0x1);
    NEW_STATE(table, "Idle09", 0x10, 0x1);
    NEW_STATE(table, "Idle10", 0x10, 0x1);
    NEW_STATE(table, "Idle11", 0x10, 0x1);
    NEW_STATE(table, "Idle12", 0x10, 0x1);
    NEW_STATE(table, "Idle13", 0x10, 0x1);
    NEW_STATE(table, "SlipIdle01", 0x10, 0x1);
    NEW_STATE(table, "Inactive01", 0x20, 0x1);
    NEW_STATE(table, "Inactive02", 0x20, 0x1);
    NEW_STATE(table, "Inactive03", 0x20, 0x1);
    NEW_STATE(table, "Inactive04", 0x20, 0x1);
    NEW_STATE(table, "Inactive05", 0x20, 0x1);
    NEW_STATE(table, "Inactive06", 0x20, 0x1);
    NEW_STATE(table, "Inactive07", 0x20, 0x1);
    NEW_STATE(table, "Inactive08", 0x20, 0x1);
    NEW_STATE(table, "Inactive09", 0x20, 0x1);
    NEW_STATE(table, "Inactive10", 0x20, 0x1);
    NEW_STATE(table, "Walk01", 0x10, 0x44);
    NEW_STATE(table, "Run01", 0x10, 0x46);
    NEW_STATE(table, "Run02", 0x10, 0x46);
    NEW_STATE(table, "Run03", 0x10, 0x46);
    NEW_STATE(table, "RunOutOfWorld01", 0x10, 0x46);
    NEW_STATE(table, "SlipRun01", 0x10, 0x46);
    NEW_STATE(table, "JumpStart01", 0x20, 0xA);
    NEW_STATE(table, "JumpLift01", 0x10, 0xA);
    NEW_STATE(table, "JumpApex01", 0x20, 0xA);
    NEW_STATE(table, "Fall01", 0x10, 0xA);
    NEW_STATE(table, "Land01", 0x20, 0);
    NEW_STATE(table, "LandRun01", 0x20, 0x6);
    NEW_STATE(table, "BounceStart01", 0x20, 0xA);
    NEW_STATE(table, "BounceLift01", 0x10, 0xA);
    NEW_STATE(table, "BounceApex01", 0x20, 0xA);
    NEW_STATE(table, "DJumpStart01", 0x20, 0xA);
    NEW_STATE(table, "DJumpLift01", 0x10, 0xA);
    NEW_STATE(table, "FallHigh01", 0x20, 0);
    NEW_STATE(table, "LandHigh01", 0x20, 0);
    NEW_STATE(table, "Bspin01", 0x20, 0x80A);
    NEW_STATE(table, "BbashAttack01", 0x10, 0x4000);
    NEW_STATE(table, "BbashStart01", 0x20, 0);
    NEW_STATE(table, "BbashStrike01", 0x20, 0x4000);
    NEW_STATE(table, "BbashMiss01", 0x20, 0);
    NEW_STATE(table, "BbounceAttack01", 0x10, 0x4000);
    NEW_STATE(table, "BbounceStart01", 0x20, 0);
    NEW_STATE(table, "BbounceStrike01", 0x20, 0);
    NEW_STATE(table, "Bbowl01", 0x20, 0);
    NEW_STATE(table, "BbowlStart01", 0x20, 0);
    NEW_STATE(table, "BbowlWindup01", 0x10, 0);
    NEW_STATE(table, "BbowlToss01", 0x20, 0);
    NEW_STATE(table, "BbowlRecover01", 0x20, 0x80);
    NEW_STATE(table, "LedgeGrab01", 0x20, 0x80);
    NEW_STATE(table, "Hit01", 0x20, 0x80);
    NEW_STATE(table, "Hit02", 0x20, 0x80);
    NEW_STATE(table, "Hit03", 0x20, 0x80);
    NEW_STATE(table, "Hit04", 0x20, 0x80);
    NEW_STATE(table, "Hit05", 0x20, 0x80);
    NEW_STATE(table, "Defeated01", 0, 0x480);
    NEW_STATE(table, "Defeated02", 0, 0x480);
    NEW_STATE(table, "Defeated03", 0, 0x480);
    NEW_STATE(table, "Defeated04", 0, 0x480);
    NEW_STATE(table, "Defeated05", 0, 0x480);
    NEW_STATE(table, "TongueSlide01", 0x10, 0x1840);
    NEW_STATE(table, "TongueStart01", 0x20, 0xA);
    NEW_STATE(table, "TongueJump01", 0x20, 0xA);
    NEW_STATE(table, "TongueJumpXtra01", 0x20, 0xA);
    NEW_STATE(table, "TongueFall01", 0x10, 0xA);
    NEW_STATE(table, "TongueLand01", 0x20, 0x1800);
    NEW_STATE(table, "TongueTumble01", 0x20, 0x1800);
    NEW_STATE(table, "Goo01", 0x10, 0);
    NEW_STATE(table, "Goo02", 0x20, 0);
    NEW_STATE(table, "GooDefeated", 0, 0x480);
    NEW_STATE(table, "WallLaunch01", 0x20, 0x8A);
    NEW_STATE(table, "WallFlight01", 0x20, 0xA);
    NEW_STATE(table, "WallFlight02", 0x10, 0xA);
    NEW_STATE(table, "WallLand01", 0x20, 0xA);
    NEW_STATE(table, "WallFall01", 0x10, 0xA);
    NEW_STATE(table, "BoulderRoll01", 0x20, 0xA);
    NEW_STATE(table, "BoulderRoll02", 0x10, 0xA);
    NEW_STATE(table, "Talk04", 0x10, 0x1);
    NEW_STATE(table, "Talk03", 0x10, 0x1);
    NEW_STATE(table, "Talk02", 0x10, 0x1);
    NEW_STATE(table, "Talk01", 0x10, 0x1);
    NEW_STATE(table, "SpatulaGrab01", 0x20, 0x80);

    NEW_TRAN(table, "Idle01 "
                    "Idle02 "
                    "Idle03 "
                    "Idle04 "
                    "Idle05 "
                    "Idle06 "
                    "Idle07 "
                    "Idle08 "
                    "Idle09 "
                    "Idle10 "
                    "Idle11 "
                    "Idle12 "
                    "Idle13 "
                    "SlipIdle01 "
                    "Inactive01 "
                    "Inactive02 "
                    "Inactive03 "
                    "Inactive04 "
                    "Inactive05 "
                    "Inactive06 "
                    "Inactive07 "
                    "Inactive08 "
                    "Inactive09 "
                    "Inactive10 "
                    "Walk01 "
                    "Run01 "
                    "Run02 "
                    "Run03 "
                    "Land01 "
                    "LandRun01", "SpatulaGrab01", SpatulaGrabCheck, SpatulaGrabCB, 0, 0, 2, 0.15f);
    NEW_TRAN(table, "SpatulaGrab01", "Idle01", NULL, SpatulaGrabStopCB, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "Idle02", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle03", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle04", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle05", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle06", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle07", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle08", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle09", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle11", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle12", "Idle01", InactiveFinishedCheck, IdleCB, 0, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive01", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive02", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive03", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive04", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive05", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive06", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive07", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive08", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive09", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Inactive10", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Land01", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.1f);
    NEW_TRAN(table, "LandHigh01", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.1f);
    NEW_TRAN(table, "Idle01 "
                    "Walk01 "
                    "Run01 "
                    "Run02 "
                    "Run03 "
                    "RunOutOfWorld01 "
                    "SlipRun01", "SlipIdle01", IdleSlipCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "LandRun01", "SlipIdle01", IdleSlipCheck, IdleCB, 0, 0, 1, 0.1f);
    NEW_TRAN(table, "SlipIdle01", "Idle01", IdleCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Walk01", "Idle01", IdleCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Run01", "Idle01", IdleCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Run02", "Idle10", IdleVictoryCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Run02", "Idle01", IdleStoicCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Idle10", "Idle01", IdleStoicCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle10", IdleVictoryCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Run03", "Idle13", IdleScaredCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Run03", "Idle01", IdleStoicCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Idle13", "Idle01", IdleStoicCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle13", IdleScaredCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "LandRun01", "Idle01", IdleCheck, IdleCB, 0, 0, 1, 0.1f);
    NEW_TRAN(table, "SlipRun01", "Idle01", IdleCheck, IdleCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle02", InactiveCheck, InactiveCB, 0, 0x140000, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle03", InactiveCheck, InactiveCB, 0, 0x140001, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle04", InactiveCheck, InactiveCB, 0, 0x140002, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle05", InactiveCheck, InactiveCB, 0, 0x140003, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle06", InactiveCheck, InactiveCB, 0, 0x140004, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle07", InactiveCheck, InactiveCB, 0, 0x140005, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle08", InactiveCheck, InactiveCB, 0, 0x140006, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle09", InactiveCheck, InactiveCB, 0, 0x140007, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle11", InactiveCheck, InactiveCB, 0, 0x140008, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Idle12", InactiveCheck, InactiveCB, 0, 0x140009, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive01", InactiveCheck, InactiveCB, 0, 0x14000A, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive02", InactiveCheck, InactiveCB, 0, 0x14000B, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive03", InactiveCheck, InactiveCB, 0, 0x14000C, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive04", InactiveCheck, InactiveCB, 0, 0x14000D, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive05", InactiveCheck, InactiveCB, 0, 0x14000E, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive06", InactiveCheck, InactiveCB, 0, 0x14000F, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive07", InactiveCheck, InactiveCB, 0, 0x140010, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive08", InactiveCheck, InactiveCB, 0, 0x140011, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive09", InactiveCheck, InactiveCB, 0, 0x140012, 1, 0.15f);
    NEW_TRAN(table, "Idle01", "Inactive10", InactiveCheck, InactiveCB, 0, 0x140013, 1, 0.15f);
    
    xAnimTransition *stdtran[8];

    stdtran[0] = NEW_TRAN(table, STANDARD_STATES[0], "Walk01", WalkCheck, NoSlipCB, 0, 0, 1, 0.15f);
    stdtran[1] = NEW_TRAN(table, STANDARD_STATES[0], "Run01", RunStoicCheck, NoSlipCB, 0, 0, 1, 0.15f);
    stdtran[2] = NEW_TRAN(table, STANDARD_STATES[0], "Run02", RunVictoryCheck, NoSlipCB, 0, 0, 3, 0.15f);
    stdtran[3] = NEW_TRAN(table, STANDARD_STATES[0], "Run03", RunScaredCheck, NoSlipCB, 0, 0, 2, 0.15f);
    stdtran[4] = NEW_TRAN(table, STANDARD_STATES[0], "SlipRun01", RunSlipCheck, SlipRunCB, 0, 0, 2, 0.15f);
    stdtran[5] = NEW_TRAN(table, STANDARD_STATES[0], "RunOutOfWorld01", RunOutOfWorldCheck, NoSlipCB, 0, 0x200100, 2, 0.15f);
    stdtran[6] = NEW_TRAN(table, STANDARD_STATES[0], "Fall01", FallCheck, NoSlipCB, 0, 0, 1, 0.15f);
    stdtran[7] = NEW_TRAN(table, STANDARD_STATES[0], "BbashStart01", BubbleBashCheck, BubbleBashCB, 0, 0, 10, 0.0f);

    for (int i = 1; STANDARD_STATES[i] != NULL; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (strcmp(STANDARD_STATES[i], stdtran[j]->Dest->Name) != 0 &&
                (i != 5 || j != 7))
            {
                ADD_TRAN(table, stdtran[j], STANDARD_STATES[i]);
            }
        }
    }

    ADD_TRAN(table, stdtran[4], "LandRun01");
    ADD_TRAN(table, stdtran[5], "LandRun01");
    ADD_TRAN(table, stdtran[6], "LandRun01");
    ADD_TRAN(table, stdtran[7], "LandRun01");

    NEW_TRAN(table, "LandRun01", "Walk01", WalkCheck, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "LandRun01", "Run01", RunStoicCheck, NULL, 0x10, 0, 1, 0.1f);
    NEW_TRAN(table, "LandRun01", "Run02", RunVictoryCheck, NULL, 0x10, 0, 3, 0.1f);
    NEW_TRAN(table, "LandRun01", "Run03", RunScaredCheck, NULL, 0x10, 0, 2, 0.1f);
    NEW_TRAN(table, "LandRun01", "RunOutOfWorld01", RunOutOfWorldCheck, NULL, 0x10, 0x200100, 3, 0.1f);
    NEW_TRAN(table, "LandRun01", "SlipRun01", RunSlipCheck, SlipRunCB, 0x10, 0, 2, 0.1f);
    
    tran = NEW_TRAN(table, "Idle01", "JumpStart01", JumpCheck, JumpCB, 0, 0, 10, 0.15f);

    ADD_TRAN(table, tran, "Walk01");
    ADD_TRAN(table, tran, "Run01");
    ADD_TRAN(table, tran, "Run02");
    ADD_TRAN(table, tran, "Run03");
    ADD_TRAN(table, tran, "RunOutOfWorld01");
    ADD_TRAN(table, tran, "SlipRun01");
    ADD_TRAN(table, tran, "SlipIdle01");
    ADD_TRAN(table, tran, "Land01");
    ADD_TRAN(table, tran, "LandHigh01");
    ADD_TRAN(table, tran, "LandRun01");
    ADD_TRAN(table, tran, "Idle02");
    ADD_TRAN(table, tran, "Idle03");
    ADD_TRAN(table, tran, "Idle04");
    ADD_TRAN(table, tran, "Idle05");
    ADD_TRAN(table, tran, "Idle06");
    ADD_TRAN(table, tran, "Idle07");
    ADD_TRAN(table, tran, "Idle08");
    ADD_TRAN(table, tran, "Idle09");
    ADD_TRAN(table, tran, "Idle10");
    ADD_TRAN(table, tran, "Idle11");
    ADD_TRAN(table, tran, "Idle12");
    ADD_TRAN(table, tran, "Idle13");
    ADD_TRAN(table, tran, "Inactive01");
    ADD_TRAN(table, tran, "Inactive02");
    ADD_TRAN(table, tran, "Inactive03");
    ADD_TRAN(table, tran, "Inactive04");
    ADD_TRAN(table, tran, "Inactive05");
    ADD_TRAN(table, tran, "Inactive06");
    ADD_TRAN(table, tran, "Inactive07");
    ADD_TRAN(table, tran, "Inactive08");
    ADD_TRAN(table, tran, "Inactive09");
    ADD_TRAN(table, tran, "Inactive10");
    ADD_TRAN(table, tran, "Goo01");

    tran = NEW_TRAN(table, "Idle01", "BounceStart01", BounceCheck, BounceCB, 0, 0, 15, 0.15f);

    ADD_TRAN(table, tran, "SlipIdle01");
    ADD_TRAN(table, tran, "Walk01");
    ADD_TRAN(table, tran, "Run01");
    ADD_TRAN(table, tran, "Run02");
    ADD_TRAN(table, tran, "Run03");
    ADD_TRAN(table, tran, "RunOutOfWorld01");
    ADD_TRAN(table, tran, "SlipRun01");
    ADD_TRAN(table, tran, "Land01");
    ADD_TRAN(table, tran, "LandHigh01");
    ADD_TRAN(table, tran, "LandRun01");
    ADD_TRAN(table, tran, "JumpStart01");
    ADD_TRAN(table, tran, "JumpApex01");
    ADD_TRAN(table, tran, "DJumpStart01");
    ADD_TRAN(table, tran, "DJumpLift01");
    ADD_TRAN(table, tran, "Fall01");
    ADD_TRAN(table, tran, "BbounceAttack01");
    ADD_TRAN(table, tran, "BbounceStart01");
    ADD_TRAN(table, tran, "BbashMiss01");

    xAnimTransition *hittran[11];

    hittran[0] = NEW_TRAN(table, HIT_STATES[0], "Hit01", Hit01Check, Hit01CB, 0, 0, 20, 0.15f);
    hittran[1] = NEW_TRAN(table, HIT_STATES[0], "Hit02", Hit02Check, Hit02CB, 0, 0, 20, 0.15f);
    hittran[2] = NEW_TRAN(table, HIT_STATES[0], "Hit03", Hit03Check, Hit03CB, 0, 0, 20, 0.15f);
    hittran[3] = NEW_TRAN(table, HIT_STATES[0], "Hit04", Hit04Check, Hit04CB, 0, 0, 20, 0.15f);
    hittran[4] = NEW_TRAN(table, HIT_STATES[0], "Hit05", Hit05Check, Hit05CB, 0, 0, 20, 0.15f);
    hittran[5] = NEW_TRAN(table, HIT_STATES[0], "Defeated01", Defeated01Check, DefeatedCB, 0, 0x5, 30, 0.15f);
    hittran[6] = NEW_TRAN(table, HIT_STATES[0], "Defeated02", Defeated02Check, DefeatedCB, 0, 0x5, 30, 0.15f);
    hittran[7] = NEW_TRAN(table, HIT_STATES[0], "Defeated03", Defeated03Check, DefeatedCB, 0, 0x5, 30, 0.15f);
    hittran[8] = NEW_TRAN(table, HIT_STATES[0], "Defeated04", Defeated04Check, DefeatedCB, 0, 0x5, 30, 0.15f);
    hittran[9] = NEW_TRAN(table, HIT_STATES[0], "Defeated05", Defeated05Check, DefeatedCB, 0, 0x5, 30, 0.15f);
    hittran[10] = NEW_TRAN(table, HIT_STATES[0], "GooDefeated", GooCheck, GooDeathCB, 0, 0, 30, 0.15f);

    for (int i = 1; HIT_STATES[i] != NULL; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            ADD_TRAN(table, hittran[j], HIT_STATES[i]);
        }
    }

    NEW_TRAN(table, "Idle01 "
                    "Idle02 "
                    "Idle03 "
                    "Idle04 "
                    "Idle05 "
                    "Idle06 "
                    "Idle07 "
                    "Idle08 "
                    "Idle09 "
                    "Idle10 "
                    "Idle11 "
                    "Idle12 "
                    "Idle13 "
                    "SlipIdle01 "
                    "Inactive01 "
                    "Inactive02 "
                    "Inactive03 "
                    "Inactive04 "
                    "Inactive05 "
                    "Inactive06 "
                    "Inactive07 "
                    "Inactive08 "
                    "Inactive09 "
                    "Inactive10 "
                    "Walk01 "
                    "Run01 "
                    "Run02 "
                    "Run03 "
                    "RunOutOfWorld01 "
                    "Land01 "
                    "LandHigh01 "
                    "LandRun01 "
                    "SlipIdle01 "
                    "SlipRun01", "TongueStart01", SlideTrackCheck, SlideTrackCB, 0, 0, 9, 0.15f);
    NEW_TRAN(table, "TongueSlide01", "Idle01", NoslideTrackCheck, NoslideTrackCB, 0, 0, 9, 0.15f);
    NEW_TRAN(table, "TongueStart01", "TongueSlide01", NULL, NULL, 0x10, 0, 0, 0.0f);
    NEW_TRAN(table, "TongueSlide01 "
                    "TongueLand01 "
                    "TongueStart01", "TongueFall01", TrackFallCheck, TrackFallCB, 0, 0, 9, 0.15f);
    NEW_TRAN(table, "TongueDJumpApex01", "TongueFall01", NULL, NULL, 0x10, 0, 0, 0.2f);
    NEW_TRAN(table, "TongueJump01", "TongueFall01", NULL, NULL, 0x10, 0, 0, 0.08f);
    NEW_TRAN(table, "TongueLand01", "TongueSlide01", NULL, NULL, 0x10, 0, 0, 0.08f);
    NEW_TRAN(table, "TongueSlide01 "
                    "TongueLand01", "TongueJump01", JumpCheck, JumpCB, 0, 0, 10, 0.15f);
    NEW_TRAN(table, "TongueJump01 "
                    "TongueFall01", "TongueJump01", TrackPrefallJumpCheck, JumpCB, 0, 0, 15, 0.15f);
    NEW_TRAN(table, "TongueFall01", "TongueDJumpApex01", DblJumpCheck, DblJumpCB, 0, 0, 10, 0.2f);
    NEW_TRAN(table, "TongueJump01 "
                    "TongueJumpXtra01 "
                    "TongueDJumpApex01 "
                    "TongueFall01", "TongueLand01", LandTrackCheck, SlideTrackCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "TongueJump01 "
                    "TongueJumpXtra01 "
                    "TongueDJumpApex01 "
                    "TongueFall01", "Land01", LandNoTrackCheck, NoslideTrackCB, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "TongueJump01 "
                    "TongueJumpXtra01 "
                    "TongueDJumpApex01 "
                    "TongueFall01", "LandRun01", LandNoTrackFastCheck, NULL, 0, 0, 3, 0.15f);
    NEW_TRAN(table, "TongueJump01 "
                    "TongueJumpXtra01 "
                    "TongueDJumpApex01 "
                    "TongueFall01", "Walk01", LandNoTrackWalkCheck, NULL, 0, 0, 3, 0.15f);
    NEW_TRAN(table, "TongueJump01 "
                    "TongueJumpXtra01 "
                    "TongueDJumpApex01 "
                    "TongueFall01", "SlipRun01", LandNoTrackSlipRunCheck, SlipRunCB, 0, 0, 4, 0.15f);
    NEW_TRAN(table, "TongueJump01 "
                    "TongueJumpXtra01 "
                    "TongueDJumpApex01 "
                    "TongueFall01", "SlipIdle01", LandNoTrackSlipIdleCheck, NULL, 0, 0, 4, 0.15f);
    NEW_TRAN(table, "TongueJump01", "TongueJumpXtra01", DblJumpCheck, TongueDblJumpCB, 0x20, 0, 10, 0.0f);
    NEW_TRAN(table, "TongueJumpXtra01", "TongueDJumpApex01", NULL, TongueDblSpinCB, 0x10, 0, 1, 0.0f);
    NEW_TRAN(table, "Idle01 "
                    "Idle02 "
                    "Idle03 "
                    "Idle04 "
                    "Idle05 "
                    "Idle06 "
                    "Idle07 "
                    "Idle08 "
                    "Idle09 "
                    "Idle10 "
                    "Idle11 "
                    "Idle12 "
                    "Idle13 "
                    "Inactive01 "
                    "Inactive02 "
                    "Inactive03 "
                    "Inactive04 "
                    "Inactive05 "
                    "Inactive06 "
                    "Inactive07 "
                    "Inactive08 "
                    "Inactive09 "
                    "Inactive10 "
                    "Walk01 "
                    "Run01 "
                    "Run02 "
                    "Run03 "
                    "RunOutOfWorld01 "
                    "SlipRun01 "
                    "SlipIdle01 "
                    "Land01 "
                    "LandRun01", "BoulderRoll01", BoulderRollCheck, BoulderRollWindupCB, 0, 0, 10, 0.5f);
    NEW_TRAN(table, "BoulderRoll01", "BoulderRoll02", NULL, BoulderRollCB, 0x10, 0, 0, 0.0f);
    NEW_TRAN(table, "BoulderRoll02", "SlipIdle01", IdleSlipCheck, NULL, 0, 0, 10, 0.25f);
    NEW_TRAN(table, "BoulderRoll02", "Idle01", IdleCheck, NULL, 0, 0, 10, 0.25f);
    NEW_TRAN(table, "BoulderRoll02", "Walk01", WalkCheck, NULL, 0, 0, 10, 0.25f);
    NEW_TRAN(table, "BoulderRoll02", "Run01", RunStoicCheck, NULL, 0, 0, 10, 0.25f);
    NEW_TRAN(table, "BoulderRoll02", "Run02", RunVictoryCheck, NULL, 0, 0, 10, 0.25f);
    NEW_TRAN(table, "BoulderRoll02", "Run03", RunScaredCheck, NULL, 0, 0, 10, 0.25f);
    NEW_TRAN(table, "BoulderRoll02", "RunOutOfWorld01", RunOutOfWorldCheck, NULL, 0, 0x200100, 10, 0.25f);
    NEW_TRAN(table, "BoulderRoll02", "SlipRun01", RunSlipCheck, SlipRunCB, 0, 0, 10, 0.25f);
    NEW_TRAN(table, "BoulderRoll02", "Fall01", FallCheck, NULL, 0, 0, 10, 0.25f);
    NEW_TRAN(table, "JumpStart01", "JumpApex01", NULL, NULL, 0x10, 0, 0, 0.08f);
    NEW_TRAN(table, "JumpApex01", "Fall01", NULL, NULL, 0x10, 0, 0, 0.08f);
    NEW_TRAN(table, "BounceStart01", "BounceLift01", NULL, NULL, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "BounceLift01", "BounceApex01", JumpApexCheck, NULL, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "BounceApex01", "Fall01", NULL, NULL, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "DJumpStart01", "DJumpLift01", NULL, NULL, 0x10, 0, 0, 0.0f);
    NEW_TRAN(table, "JumpLift01", "JumpApex01", JumpApexCheck, NULL, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "JumpStart01 "
                    "JumpLift01 "
                    "JumpApex01 "
                    "BounceStart01 "
                    "BounceLift01 "
                    "BounceApex01 "
                    "Fall01", "DJumpStart01", DblJumpCheck, DblJumpCB, 0, 0, 10, 0.15f);
    NEW_TRAN(table, "JumpStart01 "
                    "JumpLift01 "
                    "JumpApex01 "
                    "Fall01 "
                    "WallFlight01 "
                    "WallFlight02 "
                    "WallLand01 "
                    "WallFall01 "
                    "DJumpStart01 "
                    "DJumpLift01 "
                    "DJumpApex01", "WallLaunch01", WallJumpLaunchCheck, WallJumpLaunchCallback, 0, 0, 10, 0.15f);
    NEW_TRAN(table, "WallLaunch01", "WallFlight01", NULL, WallJumpCallback, 0x10, 0, 0, 0.0f);
    NEW_TRAN(table, "WallFlight01", "WallFlight02", NULL, NULL, 0x10, 0, 0, 0.0f);
    NEW_TRAN(table, "WallFlight01 "
                    "WallFlight02", "WallLand01", WallJumpFlightLandCheck, WallJumpFlightLandCallback, 0, 0, 0, 0.08f);
    NEW_TRAN(table, "WallLand01", "WallFall01", NULL, NULL, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "JumpApex01 "
                    "Fall01 "
                    "DJumpLift01 "
                    "WallFlight01 "
                    "WallFlight02 "
                    "WallLand01 "
                    "WallFall01 "
                    "BbashMiss01", "Land01", LandCheck, LandCallback, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "JumpApex01 "
                    "Fall01 "
                    "DJumpLift01 "
                    "WallFlight01 "
                    "WallFlight02 "
                    "WallLand01 "
                    "WallFall01 "
                    "BbashMiss01", "LandHigh01", LandHighCheck, LandCallback, 0, 0, 2, 0.15f);
    NEW_TRAN(table, "JumpApex01 "
                    "Fall01 "
                    "DJumpLift01 "
                    "WallFlight01 "
                    "WallFlight02 "
                    "WallLand01 "
                    "WallFall01 "
                    "BbashMiss01", "LandRun01", LandRunCheck, LandCallback, 0, 0, 3, 0.15f);
    NEW_TRAN(table, "JumpApex01 "
                    "Fall01 "
                    "DJumpLift01 "
                    "WallFlight01 "
                    "WallFlight02 "
                    "WallLand01 "
                    "WallFall01 "
                    "BbashMiss01", "SlipRun01", LandSlipRunCheck, LandSlipRunCallback, 0, 0, 4, 0.15f);
    NEW_TRAN(table, "JumpApex01 "
                    "Fall01 "
                    "DJumpLift01 "
                    "WallFlight01 "
                    "WallFlight02 "
                    "WallLand01 "
                    "WallFall01 "
                    "BbashMiss01", "SlipIdle01", LandSlipIdleCheck, LandCallback, 0, 0, 4, 0.15f);
    
    tran = NEW_TRAN(table, "Idle01", "Bspin01", BubbleSpinCheck, BubbleSpinCB, 0, 0, 10, 0.05f);

    ADD_TRAN(table, tran, "Idle02");
    ADD_TRAN(table, tran, "Idle03");
    ADD_TRAN(table, tran, "Idle04");
    ADD_TRAN(table, tran, "Idle05");
    ADD_TRAN(table, tran, "Idle06");
    ADD_TRAN(table, tran, "Idle07");
    ADD_TRAN(table, tran, "Idle08");
    ADD_TRAN(table, tran, "Idle09");
    ADD_TRAN(table, tran, "Idle10");
    ADD_TRAN(table, tran, "Idle11");
    ADD_TRAN(table, tran, "Idle12");
    ADD_TRAN(table, tran, "Idle13");
    ADD_TRAN(table, tran, "SlipIdle01");
    ADD_TRAN(table, tran, "Inactive01");
    ADD_TRAN(table, tran, "Inactive02");
    ADD_TRAN(table, tran, "Inactive03");
    ADD_TRAN(table, tran, "Inactive04");
    ADD_TRAN(table, tran, "Inactive05");
    ADD_TRAN(table, tran, "Inactive06");
    ADD_TRAN(table, tran, "Inactive07");
    ADD_TRAN(table, tran, "Inactive09");
    ADD_TRAN(table, tran, "Inactive10");
    ADD_TRAN(table, tran, "Walk01");
    ADD_TRAN(table, tran, "Run01");
    ADD_TRAN(table, tran, "Run02");
    ADD_TRAN(table, tran, "Run03");
    ADD_TRAN(table, tran, "SlipRun01");
    ADD_TRAN(table, tran, "Land01");
    ADD_TRAN(table, tran, "LandHigh01");
    ADD_TRAN(table, tran, "LandRun01");
    ADD_TRAN(table, tran, "JumpStart01");
    ADD_TRAN(table, tran, "JumpLift01");
    ADD_TRAN(table, tran, "JumpApex01");
    ADD_TRAN(table, tran, "BounceStart01");
    ADD_TRAN(table, tran, "BounceLift01");
    ADD_TRAN(table, tran, "BounceApex01");
    ADD_TRAN(table, tran, "DJumpStart01");
    ADD_TRAN(table, tran, "DJumpLift01");
    ADD_TRAN(table, tran, "Fall01");

    tran = NEW_TRAN(table, "JumpStart01", "BbounceStart01", BubbleBounceCheck, BubbleBounceCB, 0, 0, 10, 0.15f);

    ADD_TRAN(table, tran, "JumpLift01");
    ADD_TRAN(table, tran, "JumpApex01");
    ADD_TRAN(table, tran, "BounceStart01");
    ADD_TRAN(table, tran, "BounceLift01");
    ADD_TRAN(table, tran, "BounceApex01");
    ADD_TRAN(table, tran, "DJumpStart01");
    ADD_TRAN(table, tran, "DJumpLift01");
    ADD_TRAN(table, tran, "Fall01");
    ADD_TRAN(table, tran, "BbashMiss01");

    NEW_TRAN(table, "Bspin01", "SlipIdle01", IdleSlipCheck, IdleCB, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "Bspin01", "Idle01", IdleCheck, IdleCB, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "Bspin01", "Walk01", WalkCheck, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "Bspin01", "Run01", RunStoicCheck, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "Bspin01", "Run03", RunScaredCheck, NULL, 0x10, 0, 2, 0.15f);
    NEW_TRAN(table, "Bspin01", "Run02", RunVictoryCheck, NULL, 0x10, 0, 3, 0.15f);
    NEW_TRAN(table, "Bspin01", "RunOutOfWorld01", RunOutOfWorldCheck, NULL, 0x10, 0x200100, 3, 0.15f);
    NEW_TRAN(table, "Bspin01", "SlipRun01", RunSlipCheck, SlipRunCB, 0x10, 0, 4, 0.15f);
    NEW_TRAN(table, "Bspin01", "Fall01", NULL, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbashStart01", "BbashAttack01", NULL, NULL, 0x10, 0, 1, 0.0f);
    NEW_TRAN(table, "BbashStart01", "BbashStrike01", BBashStrikeCheck, BBashStrikeCB, 0, 0, 2, 0.1f);
    NEW_TRAN(table, "BbashAttack01", "BbashStrike01", BBashStrikeCheck, BBashStrikeCB, 0, 0, 2, 0.1f);
    NEW_TRAN(table, "BbashAttack01", "BbashMiss01", BBashToJumpCheck, NULL, 0, 0, 1, 0.1f);
    NEW_TRAN(table, "BbashStrike01", "Fall01", NULL, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbashMiss01", "Fall01", NULL, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbounceStart01", "BbounceAttack01", NULL, BBounceAttackCB, 0x10, 0, 1, 0.1f);
    NEW_TRAN(table, "BbounceAttack01", "BbounceStrike01", BBounceStrikeCheck, BBounceStrikeCB, 0, 0, 1, 0.0f);
    NEW_TRAN(table, "BbounceAttack01", "JumpLift01", BBounceToJumpCheck, BBounceToJumpCB, 0, 0, 1, 0.1f);
    NEW_TRAN(table, "BbounceStrike01", "SlipIdle01", IdleSlipCheck, IdleCB, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbounceStrike01", "Idle01", IdleCheck, IdleCB, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbounceStrike01", "Walk01", WalkCheck, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbounceStrike01", "Run01", RunStoicCheck, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbounceStrike01", "Run03", RunScaredCheck, NULL, 0x10, 0, 2, 0.15f);
    NEW_TRAN(table, "BbounceStrike01", "Run02", RunVictoryCheck, NULL, 0x10, 0, 3, 0.15f);
    NEW_TRAN(table, "BbounceStrike01", "RunOutOfWorld01", RunOutOfWorldCheck, NULL, 0x10, 0x200100, 3, 0.15f);
    NEW_TRAN(table, "BbounceStrike01", "SlipRun01", RunSlipCheck, SlipRunCB, 0x10, 0, 4, 0.15f);
    NEW_TRAN(table, "JumpStart01 "
                    "JumpLift01 "
                    "JumpApex01 "
                    "Fall01 "
                    "DJumpStart01 "
                    "DJumpLift01", "LedgeGrab01", LedgeGrabCheck, LedgeGrabCB, 0, 0, 11, 0.1f);
    NEW_TRAN(table, "LedgeGrab01", "Idle01", NULL, LedgeFinishCB, 0x10, 0, 0, 0.1f);
    NEW_TRAN(table, "Idle01 "
                    "Idle02 "
                    "Idle03 "
                    "Idle04 "
                    "Idle05 "
                    "Idle06 "
                    "Idle07 "
                    "Idle08 "
                    "Idle09 "
                    "Idle10 "
                    "Idle11 "
                    "Idle12 "
                    "Idle13 "
                    "SlipIdle01 "
                    "Inactive01 "
                    "Inactive02 "
                    "Inactive03 "
                    "Inactive04 "
                    "Inactive05 "
                    "Inactive06 "
                    "Inactive07 "
                    "Inactive09 "
                    "Inactive10 "
                    "Walk01 "
                    "Land01 "
                    "LandRun01 "
                    "Run01 "
                    "Run02 "
                    "Run03 "
                    "SlipRun01", "BbowlStart01", BbowlCheck, BbowlCB, 0, 0, 10, 0.1f);
    NEW_TRAN(table, "BbowlStart01", "BbowlWindup01", NULL, NULL, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbowlWindup01", "BbowlToss01", BbowlWindupEndCheck, NULL, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "BbowlToss01", "BbowlRecover01", NULL, BbowlTossEndCB, 0x10, 0, 1, 0.15f);
    NEW_TRAN(table, "BbowlRecover01", "Walk01", BbowlRecoverWalkCheck, NULL, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "BbowlRecover01", "Run01", BbowlRecoverRunCheck, NULL, 0, 0, 1, 0.15f);
    NEW_TRAN(table, "BbowlRecover01", "Run03", BbowlRecoverRunScaredCheck, NULL, 0, 0, 2, 0.15f);
    NEW_TRAN(table, "BbowlRecover01", "Run02", BbowlRecoverRunVictoryCheck, NULL, 0, 0, 3, 0.15f);
    NEW_TRAN(table, "BbowlRecover01", "RunOutOfWorld01", BbowlRecoverRunOutOfWorldCheck, NULL, 0, 0x200100, 3, 0.15f);
    NEW_TRAN(table, "BbowlRecover01", "SlipRun01", BbowlRecoverRunSlipCheck, SlipRunCB, 0, 0, 4, 0.15f);
    NEW_TRAN(table, "BbowlRecover01", "Idle01", NULL, IdleCB, 0x10, 0, 1, 0.15f);

    bungee_state::insert_animations(*table);
    cruise_bubble::insert_player_animations(*table);

    NEW_TRAN(table, "Hit01", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Hit02", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Hit03", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Hit04", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Hit05", "Idle01", NULL, IdleCB, 0x10, 0, 0, 0.15f);
    NEW_TRAN(table, "Idle01 "
                    "Idle02 "
                    "Idle03 "
                    "Idle04 "
                    "Idle05 "
                    "Idle06 "
                    "Idle07 "
                    "Idle08 "
                    "Idle09 "
                    "Idle10 "
                    "Idle11 "
                    "Idle12 "
                    "Idle13 "
                    "SlipIdle01 "
                    "Inactive01 "
                    "Inactive02 "
                    "Inactive03 "
                    "Inactive04 "
                    "Inactive05 "
                    "Inactive06 "
                    "Inactive07 "
                    "Inactive08 "
                    "Inactive09 "
                    "Inactive10 ", "Talk01", TalkCheck, NULL, 0, 0, 20, 0.15f);
    NEW_TRAN(table, "Idle01 "
                    "Idle02 "
                    "Idle03 "
                    "Idle04 "
                    "Idle05 "
                    "Idle06 "
                    "Idle07 "
                    "Idle08 "
                    "Idle09 "
                    "Idle10 "
                    "Idle11 "
                    "Idle12 "
                    "Idle13 "
                    "SlipIdle01 "
                    "Inactive01 "
                    "Inactive02 "
                    "Inactive03 "
                    "Inactive04 "
                    "Inactive05 "
                    "Inactive06 "
                    "Inactive07 "
                    "Inactive08 "
                    "Inactive09 "
                    "Inactive10 ", "Talk02", TalkCheck, NULL, 0, 0x1, 20, 0.15f);
    NEW_TRAN(table, "Idle01 "
                    "Idle02 "
                    "Idle03 "
                    "Idle04 "
                    "Idle05 "
                    "Idle06 "
                    "Idle07 "
                    "Idle08 "
                    "Idle09 "
                    "Idle10 "
                    "Idle11 "
                    "Idle12 "
                    "Idle13 "
                    "SlipIdle01 "
                    "Inactive01 "
                    "Inactive02 "
                    "Inactive03 "
                    "Inactive04 "
                    "Inactive05 "
                    "Inactive06 "
                    "Inactive07 "
                    "Inactive08 "
                    "Inactive09 "
                    "Inactive10 ", "Talk03", TalkCheck, NULL, 0, 0x2, 20, 0.15f);
    NEW_TRAN(table, "Idle01 "
                    "Idle02 "
                    "Idle03 "
                    "Idle04 "
                    "Idle05 "
                    "Idle06 "
                    "Idle07 "
                    "Idle08 "
                    "Idle09 "
                    "Idle10 "
                    "Idle11 "
                    "Idle12 "
                    "Idle13 "
                    "SlipIdle01 "
                    "Inactive01 "
                    "Inactive02 "
                    "Inactive03 "
                    "Inactive04 "
                    "Inactive05 "
                    "Inactive06 "
                    "Inactive07 "
                    "Inactive08 "
                    "Inactive09 "
                    "Inactive10 ", "Talk04", TalkCheck, NULL, 0, 0x3, 20, 0.15f);
    NEW_TRAN(table, "Talk01", "Idle01", TalkDoneCheck, NULL, 0, 0, 20, 0.15f);
    NEW_TRAN(table, "Talk02", "Idle01", TalkDoneCheck, NULL, 0, 0x1, 20, 0.15f);
    NEW_TRAN(table, "Talk03", "Idle01", TalkDoneCheck, NULL, 0, 0x2, 20, 0.15f);
    NEW_TRAN(table, "Talk04", "Idle01", TalkDoneCheck, NULL, 0, 0x3, 20, 0.15f);

    return table;
}

xAnimTable *zSpongeBobTongue_AnimTable()
{
    BFBBSTUB("zSpongeBobTongue_AnimTable");
    return NULL;
}

xAnimTable *zEntPlayer_BoulderVehicleAnimTable()
{
    BFBBSTUB("zEntPlayer_BoulderVehicleAnimTable");
    return NULL;
}

xAnimTable *zEntPlayer_TreeDomeSBAnimTable()
{
    BFBBSTUB("zEntPlayer_TreeDomeSBAnimTable");
    return NULL;
}

#endif
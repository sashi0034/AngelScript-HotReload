class Team {
    FighterRef leader;
    Team@ target;
}

class TeamEx : Team {
    int d;
}

class Context {
    int reloadCount = 0;
    Team@[] teams;
}

int g_d = 696;
Context g_ctx;

int tick_frame() {
    // 超適当な謎の疑似戦闘
    if (g_ctx.teams.length() == 0) {
        TeamEx team;
        team.d = g_d;

        team.leader.data().name = "Initial Leader";
        team.leader.data().life = 99;

        @team.target = Team();
        team.target.leader.data().name = "Initial TGT";
        team.target.leader.data().life = 100;

        g_ctx.teams.insertLast(team);
    } else {
        g_ctx.reloadCount++;
        g_ctx.teams[0].leader.data().life -= 1;

        g_ctx.teams[0].target.leader.data().life -= 2;

        Team t;
        t.leader.data().life = 50;
        t.leader.data().name = "Fighter [" + g_ctx.reloadCount + "]";

        @t.target = g_ctx.teams[0];

        g_ctx.teams.insertAt(0, t);
    }

    if (g_ctx.teams.length() >= 3) {
        TeamEx t;
        t.leader.data().life = 50;
        t.leader.data().name = "Fighter [" + g_ctx.reloadCount + "]";

        @t.target = g_ctx.teams[0];
        g_d++;
        t.d = g_d;

        g_ctx.teams.insertAt(0, t);

        g_ctx.teams.removeRange(1, g_ctx.teams.length() - 1);
    }

    for (uint i = 0; i < g_ctx.teams.length(); i++) {
        auto@ leader = g_ctx.teams[i].leader;
        println("[" + i + "] " + leader.data().name + ": " + leader.data().life);

        auto@ targetLeader = g_ctx.teams[i].target.leader;
        println("- TGT: " + targetLeader.data().name + ": " + targetLeader.data().life);

        TeamEx@ teamEx = cast<TeamEx>(g_ctx.teams[i]);
        if (teamEx !is null) {
            println(" - d: " + teamEx.d);
            teamEx.d++;
        } else {
            println(" - d: n/a");
        }
    }

    return g_ctx.reloadCount;
}

// AngelScript のハンドル @ は、ポインタと言うより後から取っ付け可能な参照渡しと考えるといいかも

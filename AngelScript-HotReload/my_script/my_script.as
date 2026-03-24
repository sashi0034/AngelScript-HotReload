class Team {
    FighterRef leader;
    Team@ target;
}

class Context {
    Team[] teams;
}

Context g_ctx;

int tick_frame() {
    if (g_ctx.teams.length() == 0) {
        Team team;

        team.leader.data().name = "Initial Leader";
        team.leader.data().life = 99;

        @team.target = Team();
        team.target.leader.data().name = "Initial TGT";
        team.target.leader.data().life = 100;

        g_ctx.teams.insertLast(team);
    } else {
        g_ctx.teams[0].leader.data().life -= 1;
        g_ctx.teams[0].target.leader.data().life -= 2;
    }

    for (uint i = 0; i < g_ctx.teams.length(); i++) {
        auto@ leader = g_ctx.teams[i].leader;
        println("[" + i + "] " + leader.data().name + ": " + leader.data().life);

        auto@ targetLeader = g_ctx.teams[i].target.leader;
        println("- TGT: " + targetLeader.data().name + ": " + targetLeader.data().life);
    }

    return 123;
}

// AngelScript のハンドル @ は、ポインタと言うより後から取っ付け可能な参照渡しと考えるといいかも

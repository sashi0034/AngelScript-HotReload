
class Context {
    FighterRef@[] fighter;
}

int tick_frame() {
    Context ctx;

    FighterRef f(Fighter());
    f.get().name = "My Fighter";

    ctx.fighter.insertLast(f);

    println(ctx.fighter[0].get().name);

    println("random value: " + rand(123));

    return 123;
}

// AngelScript のハンドル @ は、ポインタと言うより後から取っ付け可能な参照渡しと考えるといいかも

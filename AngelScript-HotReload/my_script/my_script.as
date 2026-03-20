
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


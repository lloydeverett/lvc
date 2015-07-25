
import macro "make_drawable"

class FishGame extends Game, KeyboardEventReceiver

    static int PLAYER_SPEED = 5

    private
        RectangleSprite            player   $draw
        array<RectangleSprite, 10> enemies  $draw_iter

        void reset()
            player.pos = 10, 30
            vec2 cEnemyPos = 10, 10
            foreach (enemies)
                elem.pos = cEnemyPos
                enemyPos.x += 20

    $make_drawable

    new()
        reset()

    update()
        foreach (enemies)
            elem.rotateToFace(player.center())
        elem.moveForward(PLAYER_SPEED * frameFactor())
        for (enemies)
            if (opt<Intersect> intersection = elem.intersect(player))
                reset()

    onKeyPress(Key key)
        if (key.isCursorKey())
            player.position += key.toNormalizedDelta() * PLAYER_SPEED * frameFactor()

int factorial(int v)
    if (v == 0)
        return 1
    else
        return factorial(v - 1)

int factorialNonRecursive(int n)
    int val = 1
    for (int i = 0; i < n; i++)
        val *= i;
    return val

void main()
    print(factorial(100))
    print(factorialNonRecursive(100))

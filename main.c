#include <raylib.h>
#include <raymath.h>

// === Constants & Definitions ===
#define windowWidth 1200
#define windowHeight 800

typedef enum Scene { StartScreen, GameScene, EndScreen } Scene;

// === Structs ===

typedef struct PaddleControls {
  int key_Up, key_Down;
} PaddleControls;

typedef struct Paddle {
  int playerID;
  float width, height, xPos, yPos;
  PaddleControls controls;
  Rectangle body;
  Color color;
} Paddle;

typedef struct Ball {
  float radius;
  Vector2 position, velocity;
  Color color;
} Ball;

// === Constants ===
int winner = 0;

Scene currentScene = StartScreen;

Color ballColor = BLUE;
Color paddleColor = BLACK;

const float paddleSpeed = 6.0f;
const float paddleInitialWidth = 15.0f;
const float paddleInitialHeight = 120.0f;

const float paddle1InitialPosX = 30.0f;
const float paddle1InitialPosY =
    windowHeight / 2.0f - paddleInitialHeight / 2.0f;
const float paddle2InitialPosX =
    windowWidth - paddle1InitialPosX - paddleInitialWidth;
const float paddle2InitialPosY =
    windowHeight / 2.0f - paddleInitialHeight / 2.0f;

const float ballSpeed = 10.0f;
const float ballInitialRadius = 10.0f;

// === Functions ===

void UpdatePaddle(Paddle *paddle) {
  // - Input -
  if (IsKeyDown(paddle->controls.key_Up)) {
    paddle->yPos -= paddleSpeed;
  }
  if (IsKeyDown(paddle->controls.key_Down)) {
    paddle->yPos += paddleSpeed;
  }

  // - Wall Constraints -
  if (paddle->yPos < 0)
    paddle->yPos = 0;
  if (paddle->yPos + paddle->height > windowHeight)
    paddle->yPos = windowHeight - paddle->height;

  // Sync collision rectangle with updated position
  paddle->body.y = paddle->yPos;
}

void UpdateBall(Ball *ball, Paddle *p1, Paddle *p2) {
  // Basic Movement
  ball->position.x += ball->velocity.x;
  ball->position.y += ball->velocity.y;

  // Speed Clamping
  if (ball->velocity.x > ballSpeed)
    ball->velocity.x = ballSpeed;
  if (ball->velocity.y > ballSpeed)
    ball->velocity.y = ballSpeed;

  // Wall Collision
  if (ball->position.y - ball->radius < 0) {
    ball->position.y = ball->radius;
    ball->velocity.y = -ball->velocity.y;
  }
  if (ball->position.y + ball->radius > windowHeight) {
    ball->position.y = windowHeight - ball->radius;
    ball->velocity.y = -ball->velocity.y;
  }

  // Game End Condition
  if (ball->position.x < 0)
    winner = 2;
  else if (ball->position.x > windowWidth)
    winner = 1;

  // Paddle Collision
  if (CheckCollisionCircleRec(ball->position, ball->radius, p1->body)) {
    ball->velocity.x = fabsf(ball->velocity.x);
    ball->position.x = p1->xPos + p1->width + ball->radius;
  }
  if (CheckCollisionCircleRec(ball->position, ball->radius, p2->body)) {
    ball->velocity.x = -fabsf(ball->velocity.x);
    ball->position.x = p2->xPos - ball->radius;
  }
}

void ResetGameState(Paddle *p1, Paddle *p2, Ball *ball) {
  p1->yPos = paddle1InitialPosY;
  p1->body.y = paddle1InitialPosY;
  p2->yPos = paddle2InitialPosY;
  p2->body.y = paddle2InitialPosY;

  ball->position = (Vector2){windowWidth / 2.0f, windowHeight / 2.0f};
  ball->velocity = (Vector2){-ballSpeed, (float)GetRandomValue(-5, 5)};
  winner = 0;
}

void DrawIntroScene(void) {
  const char *msg = "Press SPACE to start";
  int fontSize = 30;
  DrawText(msg, windowWidth / 2 - MeasureText(msg, fontSize) / 2,
           windowHeight / 2 - fontSize / 2, fontSize, BLACK);
}

void DrawEndScene(void) {
  const char *winMsg = (winner == 1) ? "Player 1 Wins!" : "Player 2 Wins!";
  const char *restartMsg = "Press SPACE to play again";
  int fontSize = 30;

  DrawText(winMsg, windowWidth / 2 - MeasureText(winMsg, fontSize) / 2,
           windowHeight / 2 - 40, fontSize, BLACK);
  DrawText(restartMsg, windowWidth / 2 - MeasureText(restartMsg, fontSize) / 2,
           windowHeight / 2 + 10, fontSize, DARKGRAY);
}

int main(void) {
  InitWindow(windowWidth, windowHeight, "Pong");
  SetTargetFPS(60);

  Paddle paddle1 = {.playerID = 1,
                    .xPos = paddle1InitialPosX,
                    .yPos = paddle1InitialPosY,
                    .color = paddleColor,
                    .width = paddleInitialWidth,
                    .height = paddleInitialHeight,
                    .body = {.height = paddleInitialHeight,
                             .width = paddleInitialWidth,
                             .x = paddle1InitialPosX,
                             .y = paddle1InitialPosY},
                    .controls = {.key_Up = KEY_W, .key_Down = KEY_S}};

  Paddle paddle2 = {.playerID = 2,
                    .xPos = paddle2InitialPosX,
                    .yPos = paddle2InitialPosY,
                    .color = paddleColor,
                    .width = paddleInitialWidth,
                    .height = paddleInitialHeight,
                    .body = {.height = paddleInitialHeight,
                             .width = paddleInitialWidth,
                             .x = paddle2InitialPosX,
                             .y = paddle2InitialPosY},
                    .controls = {.key_Up = KEY_UP, .key_Down = KEY_DOWN}};

  Ball ball = {.position = {windowWidth / 2.0f, windowHeight / 2.0f},
               .radius = ballInitialRadius,
               .velocity = {-ballSpeed, (float)GetRandomValue(-5, 5)},
               .color = ballColor};

  while (!WindowShouldClose()) {
    // Game Logic
    switch (currentScene) {
    case StartScreen: {
      if (IsKeyPressed(KEY_SPACE)) {
        currentScene = GameScene;
      }
      break;
    }

    case GameScene: {
      UpdatePaddle(&paddle1);
      UpdatePaddle(&paddle2);
      UpdateBall(&ball, &paddle1, &paddle2);
      if (winner != 0) {
        currentScene = EndScreen;
      }
      break;
    }

    case EndScreen: {
      if (IsKeyPressed(KEY_SPACE)) {
        ResetGameState(&paddle1, &paddle2, &ball);
        currentScene = GameScene;
      }
      break;
    }
    }

    // === Drawing ===
    BeginDrawing();
    ClearBackground(WHITE);

    switch (currentScene) {
    case StartScreen: {
      DrawIntroScene();
      break;
    }
    case GameScene: {
      DrawRectangle(paddle1.xPos, paddle1.yPos, paddle1.width, paddle1.height,
                    paddle1.color);
      DrawRectangle(paddle2.xPos, paddle2.yPos, paddle2.width, paddle2.height,
                    paddle2.color);
      DrawCircle(ball.position.x, ball.position.y, ball.radius, ball.color);
      break;
    }
    case EndScreen: {
      DrawEndScene();
      break;
    }
    }
    EndDrawing();
  }

  // Cleanup
  CloseWindow();
  return 0;
}
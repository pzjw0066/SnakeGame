#include "raylib.h"
#include <deque>
#include <random>
#include <ctime>
#include <utility>
#include <list>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <thread>

using namespace std;

namespace
{
	const int WIDTH = 400,HEIGHT = 300;
	
	struct snake
	{
		static const Vector2 RIGHT,LEFT,UP,DOWN;
		static const int BODY_SIZE;
		snake():
		body(1,Vector2{WIDTH/2,HEIGHT/2}),
		maxlength{1},
		speed(RIGHT){}
		void addbody()
		{
			maxlength++;
		}
		void move()
		{
			Vector2 head = *body.rbegin();
			head.x += speed.x;
			head.y += speed.y;
			body.push_back(head);
			if (body.size() > maxlength)
				body.pop_front();
		}
		void turn(Vector2 direction)
		{
			if (direction.x + speed.x != 0 || direction.y + speed.y != 0)
				speed = direction;
		}
		pair<deque<Vector2>::const_iterator,deque<Vector2>::const_iterator> getbody()
		{
			return {body.cbegin(),body.cend()};
		}
		size_t getlength()
		{
			return body.size();
		}
		private:
		deque<Vector2> body;
		Vector2 speed;
		int maxlength;
	};
	
	const int snake::BODY_SIZE{10};
	const Vector2 snake::RIGHT{snake::BODY_SIZE,0};
	const Vector2 snake::LEFT{-snake::BODY_SIZE,0};
	const Vector2 snake::UP{0,-snake::BODY_SIZE};
	const Vector2 snake::DOWN{0,snake::BODY_SIZE};
	
	bool collision_self(pair<deque<Vector2>::const_iterator,deque<Vector2>::const_iterator> &body,size_t length)
	{
		for (auto ita=body.first;ita!=body.second;ita++)
			for (auto itb=ita+1;itb!=body.second;itb++)
				if (ita->x == itb->x && ita->y == itb->y)
					return true;
		return false;
	}
	
	bool is_outside(pair<deque<Vector2>::const_iterator,deque<Vector2>::const_iterator> &body)
	{
		return body.first->x < 0 || body.first->y < 0 || body.first->x + snake::BODY_SIZE > WIDTH ||  body.first->y + snake::BODY_SIZE > HEIGHT;
	}
}

int main()
{	
	snake *player = new snake;
	list<Vector2> food;
	default_random_engine engine((unsigned int)time(nullptr));
	uniform_int_distribution<int> xdbt(0,WIDTH),ydbt(0,HEIGHT),try_generade_food(0,100);
	bool is_running = true;

	InitWindow(WIDTH,HEIGHT,"Snake game");
	SetTargetFPS(10);
	while (!WindowShouldClose())
	{
		if (is_running)
		{
			player->move();
			
			if (IsKeyPressed(KEY_A))
				player->turn(snake::LEFT);
			else if (IsKeyPressed(KEY_D))
				player->turn(snake::RIGHT);
			else if (IsKeyPressed(KEY_W))
				player->turn(snake::UP);
			else if (IsKeyPressed(KEY_S))
				player->turn(snake::DOWN);
			
			if (try_generade_food(engine) == 0)
			{
				food.push_back(Vector2{static_cast<float>(xdbt(engine)),static_cast<float>(ydbt(engine))});
			}
			
			auto body = player->getbody();
			
			for (auto it=food.begin();it!=food.end();it++)
			{
				if (CheckCollisionCircleRec(*it,5,{(*(body.second-1)).x,(*(body.second-1)).y,snake::BODY_SIZE,snake::BODY_SIZE}))
				{
					player->addbody();
					food.erase(it);
				}
				else
					DrawCircleV(*it,5,YELLOW);
			}
			if (collision_self(body,player->getlength()) || is_outside(body))
			{
				is_running = false;
				delete player;
				food.clear();
			}
			else
			{
				BeginDrawing();
					ClearBackground(BLACK);
					for (auto it=body.first;it!=body.second;it++)
						DrawRectangleV(*it,{snake::BODY_SIZE,snake::BODY_SIZE},WHITE);
					DrawText((string("score: ")+to_string(player->getlength()-1)).c_str(),0,0,20,RED);
				EndDrawing();
			}
		}
		else
		{
			BeginDrawing();
				ClearBackground(BLACK);
				DrawText("GAME OVER!",WIDTH/2-50,HEIGHT/2,20,RED);
			EndDrawing();
			if (IsMouseButtonReleased(0))
			{
				is_running = true;
				player = new snake;
			}
		}
	}
	CloseWindow();
	return 0;
}

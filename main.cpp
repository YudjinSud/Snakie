#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <ctime>
#include <deque>
#include <windows.h>


#define fieldSize 400
#define cellSize 10
#define step 10
#define foodSize 10
#define foodIntersectionEpsilon 20
#define latency 50

std::mt19937 gen(time(0));//Mersenne twister
std::uniform_int_distribution<> foodDistributionX(cellSize, fieldSize - cellSize);
std::uniform_int_distribution<> foodDistributionY(cellSize, fieldSize - cellSize);


enum collisionType {FOOD, CELL}; // which entity we collide with ? 


struct Cell {
	sf::RectangleShape rect;
	int x;
	int y;
	int direction;
};


struct Snake {
	std::deque<Cell*> body;
};


struct Food{
	sf::CircleShape shape;
	int x;
	int y;
	boolean foodOnField; // check if there is already some piece of food snake to eat
};


void printDebugSnake(Snake *snake) {
	//std::cout << "Tail :: " << snake->tail << "\n";
}


void drawCell(sf::RenderWindow &window, Cell *cell, int n) {
	//why pointer - cell* ?
	//for now i suppose there will be a lot of cells - certainly.
	//And every game loop we have to pass all that data to draw() and update() stuff
	cell->rect.setFillColor(sf::Color::Magenta);
	cell->rect.setOutlineColor(sf::Color::Black);
	cell->rect.setOutlineThickness(1);
	cell->rect.setPosition(cell->x, cell->y);
	cell->rect.setSize(sf::Vector2f(cellSize, cellSize));
	window.draw(cell->rect);
}


void drawSnake(sf::RenderWindow &window, Snake *snake) {
	for (int i = 0; i < snake->body.size(); i++) {
		drawCell(window, snake->body[i], i);
	}
//	window.display();
}


void addCell(Snake *snake) {
	Cell *push = new Cell;
	Cell *tail = snake->body[snake->body.size() - 1];
	push->x = tail->x - cellSize;
	push->y = tail->y;
	push->direction = 0;
	snake->body.push_back(push);
}


int processKeyboard(sf::Event event, sf::RenderWindow &window) {
	//returns direction of snake
	int direction = 0;
	switch (event.key.code) {
	case sf::Keyboard::Up:
		direction = 1;
		break;
	case sf::Keyboard::Down:
		direction = 2;
		break;
	case sf::Keyboard::Left:
		direction = 3;
		break;
	case sf::Keyboard::Right:
		direction = 4;
		break;
	default:
		window.close();
	}
	return direction;
}


void copyLastToFirst(Snake *snake) {
	//only we have to do to animate snake - delete tail and insert it right on the head of sneak
	int size = snake->body.size();
	Cell *push = new Cell;
	push->direction = snake->body[0]->direction;
	push->rect = snake->body[0]->rect;
	push->x  = snake->body[0]->x;
	push->y = snake->body[0]->y;
	snake->body.push_front(push);
	snake->body.pop_back();
}


int checkYBoundary(Cell *head) {
	//check if snake is out of game field for y coords
	if (head->y > fieldSize - cellSize)
	{
		return -fieldSize;
	}
	else if (head->y < -cellSize)
	{
		return -fieldSize;
	}
	return step;
}


int checkXBoundary(Cell *head) {
	if (head->x > fieldSize - cellSize)
	{
		return -fieldSize;
	}
	else if (head->x < -cellSize)
	{
		return -fieldSize;
	}
	return step;
}


void updateSnakeCells(Snake *snake) {
	int deltaX = checkXBoundary(snake->body.front());
	int deltaY = checkYBoundary(snake->body.front());
	copyLastToFirst(snake);
	switch (snake->body[0]->direction) {
		case 1:
			snake->body[0]->y -= deltaY;
			break;
		case 2:
			snake->body[0]->y += deltaY;
			break;
		case 3:
			snake->body[0]->x -= deltaX;
			break;
		case 4:
			snake->body[0]->x += deltaX;
			break;
	}
}


int checkCellsRelation(Cell *c1, Cell *c2) {
	//given head of snake and 2'nd cell of snake
	//return the direction of 2'nd according to position of first

	if (c1->x == c2->x && c1->y - cellSize == c2->y) {
		return 1; //above
	}
	else if (c1->x == c2->x && c1->y + cellSize == c2->y) {
		return 2; // below
	}
	else if (c1->x - cellSize == c2->x && c1->y == c2->y) {
		return 3; // leftward
	}
	else if(c1->x + cellSize == c2->x && c1->y == c2->y){
		return 4; //right
	}
}


void rotateHead(Snake *snake, int direction) {
	//need to check if direction is possible
	int relation12 = checkCellsRelation(snake->body[0], snake->body[1]);
	int relation23 = checkCellsRelation(snake->body[1], snake->body[2]); // check if right or left rotation is even possible
	// because maybe there some 3'd cell;
	int x = snake->body.front()->x;
	int y = snake->body.front()->y;
	if (x <= 0 || x >= fieldSize || y <= 0 || y >= fieldSize) {
		//while head is out field not allowed to rotate
		return;
	}
	if (direction == 1 && relation12 != 1) //up
	{
		snake->body[0]->direction = direction;
	}
	else if (direction == 2 && relation12 != 2) //down
	{
		snake->body[0]->direction = direction;
	}
	else if (direction == 3 && relation12 != 3 && relation23 != 3) // left
	{
		snake->body[0]->direction = direction;
	}
	else if (direction == 4 && relation12 != 4 && relation23 != 4) // right
	{
		snake->body[0]->direction = direction;
	}
}


//@utility
void fillSnake(Snake *snake, int n) {
	for (int i = 0; i < n; i++) {
		addCell(snake);
	}
}


void drawFood(Food *piece, sf::RenderWindow &window) {
	piece->shape.setPosition(sf::Vector2f(piece->x, piece->y));
	piece->shape.setFillColor(sf::Color::Red);
	piece->shape.setRadius(foodSize);
	window.draw(piece->shape);
}



void generateFood(Food *food ,int x, int y) {
	std::cout << " X :: " << x << " Y :: " << y;
	food->x = x;
	food->y = y;
}


int getDistance(int x1, int y1, int x2, int y2) {
	double dx = x1 - x2;
	double dy = y1 - y2;
	return sqrt(dx * dx + dy * dy);
}


int collisionDetection(Cell *head, int x, int y, int r, collisionType t) {
	// x, y, r - params of entity to check with
	// if checking with other cell, r just = 0

	double l = getDistance(head->x, head->y, x+r, y+r);
	switch (t) {
		case FOOD :
			if (l < foodIntersectionEpsilon) {
				std::cout << "collision with food\n";
				return 1;
			}
			break;
		case CELL:
			if (l == 0) {
				std::cout << "collision with cell\n";
				return 1;
			}
	}
	return 0;
}


void updateFood(Food *food) {
	int x, y;
	if (!(food->foodOnField)) {
		x = foodDistributionX(gen);
		y = foodDistributionY(gen);
		generateFood(food, x, y);
		food->foodOnField = true;
	}
}


void updateGameWorld(Snake *snake, Food *food) {
	std::cout << "\r" << snake->body[0]->x << " " << snake->body[0]->y;
	updateSnakeCells(snake);
	updateFood(food);
	collisionType f = FOOD;
	collisionType c = CELL;
	if (collisionDetection(snake->body[0], food->x, food->y, food->shape.getRadius(), f)) {
		addCell(snake);
		food->foodOnField = false;
	}
	for (int i = 1; i < snake->body.size(); i++) {
		if (collisionDetection(snake->body[0], snake->body[i]->x, snake->body[i]->y, 0, c)) {
			exit(-1);
		}
	}
}



void proccesGameInput(sf::RenderWindow &window, Snake *snake, sf::Event event) {
	int direction;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();
		if (event.type == sf::Event::KeyPressed) {
			direction = processKeyboard(event, window);
			if (snake->body[0]->direction != direction) {
				rotateHead(snake, direction);
			}
		}
	}
}



void drawGame(Snake *snake, Food *food, sf::RenderWindow &window) {
	drawSnake(window, snake);
	drawFood(food, window);
	window.display();
	window.clear();
	sleep(sf::milliseconds(latency));
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(fieldSize, fieldSize), "Snakie!");
	sf::Event event;
	window.pollEvent(event);


	Cell *cell1 = new Cell;
	cell1->x = 100;
	cell1->y = 100;
	cell1->direction = 4;

	Snake *snake = new Snake; //again, we don't know how much of stack available here. Using heap therefore.
	snake->body = std::deque<Cell*>();
	snake->body.push_back(cell1); 

	fillSnake(snake, 20);

	drawSnake(window, snake);

	Food *food = new Food;
	food->foodOnField = false;

	int direction = 0;

	while (window.isOpen()) {
		proccesGameInput(window, snake, event);
		updateGameWorld(snake, food);
		drawGame(snake, food, window);
	}

	return 0;
}
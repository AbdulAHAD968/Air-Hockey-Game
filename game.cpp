#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<sstream>
#include<iostream>
#include<vector>

using namespace std;

int main(){

    sf::RenderWindow window(sf::VideoMode(800, 600), "AIR HOCKEY");
    window.setFramerateLimit(60);

    // Background
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("1.jpg")) {
        cerr << "Error loading background texture\n";
        return -1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // Paddle 1
    sf::RectangleShape PAD_01(sf::Vector2f(20, 100));
    PAD_01.setFillColor(sf::Color::Blue);
    PAD_01.setPosition(50, 250);

    sf::RectangleShape PAD_01_GLOW(sf::Vector2f(40, 120));
    PAD_01_GLOW.setFillColor(sf::Color(0, 0, 255, 100)); // Blue glow
    PAD_01_GLOW.setPosition(40, 240);

    // Paddle 2
    sf::RectangleShape PAD_02(sf::Vector2f(20, 100));
    PAD_02.setFillColor(sf::Color::Red);
    PAD_02.setPosition(730, 250);

    sf::RectangleShape PAD_02_GLOW(sf::Vector2f(40, 120));
    PAD_02_GLOW.setFillColor(sf::Color(255, 0, 0, 100)); // Red glow
    PAD_02_GLOW.setPosition(720, 240);

    // Ball
    sf::CircleShape BALL(15);
    BALL.setFillColor(sf::Color::White);
    BALL.setPosition(392, 292);

    sf::CircleShape BALL_GLOW(25);
    BALL_GLOW.setFillColor(sf::Color(255, 255, 255, 50));
    BALL_GLOW.setPosition(382, 282);

    // Borders and Goals
    sf::RectangleShape TOP_BORDER(sf::Vector2f(800, 10));
    TOP_BORDER.setFillColor(sf::Color::Green);
    sf::RectangleShape BOTTOM_BORDER(sf::Vector2f(800, 10));
    BOTTOM_BORDER.setFillColor(sf::Color::Green);
    BOTTOM_BORDER.setPosition(0, 590);
    sf::RectangleShape LEFT_GOAL(sf::Vector2f(10, 100));
    LEFT_GOAL.setFillColor(sf::Color::Black);
    LEFT_GOAL.setPosition(0, 250);
    sf::RectangleShape RIGHT_GOAL(sf::Vector2f(10, 100));
    RIGHT_GOAL.setFillColor(sf::Color::Black);
    RIGHT_GOAL.setPosition(790, 250);

    // Ball velocity
    sf::Vector2f BALL_VELOCITY(5.f, 3.f);
    float AI_SPEED = 3.5f;

    // Scores and Timer
    int SCORE_1 = 0;
    int SCORE_2 = 0;
    sf::Clock GAME_CLOCK;
    const float GAME_TIMER = 60.f;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Error loading font file.\n";
        return -1;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Cyan);
    scoreText.setPosition(10, 10);

    sf::Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::Yellow);
    timerText.setPosition(660, 10);

    sf::SoundBuffer hitBuffer, goalBuffer;
    if (!hitBuffer.loadFromFile("hit.wav") || !goalBuffer.loadFromFile("goal.wav")) {
        cerr << "Error loading sound files\n";
        return -1;
    }
    sf::Sound hitSound(hitBuffer);
    sf::Sound goalSound(goalBuffer);

    std::vector<sf::CircleShape> ballTrail;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Game time check
        float USED_TIME = GAME_CLOCK.getElapsedTime().asSeconds();
        if (USED_TIME >= GAME_TIMER) {
            std::stringstream SHOW_RESULTS;
            SHOW_RESULTS << "[ GAME OVER ]\nSCORES::\n[ BLUE:: " << SCORE_1 << " ]\n[ RED:: " << SCORE_2 << " ]\n";
            sf::Text resultText(SHOW_RESULTS.str(), font, 30);
            resultText.setFillColor(sf::Color::White);
            resultText.setPosition(100, 250);
            window.clear();
            window.draw(resultText);
            window.display();
            sf::sleep(sf::seconds(5));
            window.close();
            break;
        }

        // Player paddle movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && PAD_01.getPosition().y > 10) {
            PAD_01.move(0, -5);
            PAD_01_GLOW.move(0, -5);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && PAD_01.getPosition().y + PAD_01.getSize().y < 590) {
            PAD_01.move(0, 5);
            PAD_01_GLOW.move(0, 5);
        }

        // AI paddle movement
        float GET_PADDLE_POS = PAD_02.getPosition().y + PAD_02.getSize().y / 2;
        float GET_BALL_POS = BALL.getPosition().y + BALL.getRadius();
        if (abs(GET_PADDLE_POS - GET_BALL_POS) > 5) {
            if (GET_PADDLE_POS < GET_BALL_POS && PAD_02.getPosition().y + PAD_02.getSize().y < 590) {
                PAD_02.move(0, AI_SPEED);
                PAD_02_GLOW.move(0, AI_SPEED);
            } else if (GET_PADDLE_POS > GET_BALL_POS && PAD_02.getPosition().y > 10) {
                PAD_02.move(0, -AI_SPEED);
                PAD_02_GLOW.move(0, -AI_SPEED);
            }
        }

        // Ball movement
        BALL.move(BALL_VELOCITY);
        BALL_GLOW.move(BALL_VELOCITY);

        // Ball collision with walls
        if (BALL.getPosition().y <= 10 || BALL.getPosition().y + BALL.getRadius() * 2 >= 590) {
            BALL_VELOCITY.y = -BALL_VELOCITY.y;
            hitSound.play();
        }

        // Ball collision with paddles
        if (BALL.getGlobalBounds().intersects(PAD_01.getGlobalBounds())) {
            BALL_VELOCITY.x = abs(BALL_VELOCITY.x);
            BALL.setPosition(PAD_01.getPosition().x + PAD_01.getSize().x, BALL.getPosition().y);
            hitSound.play();
        } else if (BALL.getGlobalBounds().intersects(PAD_02.getGlobalBounds())) {
            BALL_VELOCITY.x = -abs(BALL_VELOCITY.x);
            BALL.setPosition(PAD_02.getPosition().x - BALL.getRadius() * 2, BALL.getPosition().y);
            hitSound.play();
        }

        // Check goals
        if (BALL.getPosition().x <= 0 && BALL.getPosition().y + BALL.getRadius() > LEFT_GOAL.getPosition().y &&
            BALL.getPosition().y < LEFT_GOAL.getPosition().y + LEFT_GOAL.getSize().y) {
            SCORE_2 += 10;
            BALL.setPosition(392, 292);
            BALL_VELOCITY = {5.f, 3.f};
            goalSound.play();
        }

        if (BALL.getPosition().x + BALL.getRadius() * 2 >= 800 && BALL.getPosition().y + BALL.getRadius() > RIGHT_GOAL.getPosition().y &&
            BALL.getPosition().y < RIGHT_GOAL.getPosition().y + RIGHT_GOAL.getSize().y) {
            SCORE_1 += 10;
            BALL.setPosition(392, 292);
            BALL_VELOCITY = {-5.f, -3.f};
            goalSound.play();
        }

        // Ball trail effect
        sf::CircleShape trail = BALL;
        trail.setFillColor(sf::Color(255, 255, 255, 100));
        ballTrail.push_back(trail);
        if (ballTrail.size() > 10) ballTrail.erase(ballTrail.begin());

        // Update score and time
        std::stringstream LIVE_SCORE;
        LIVE_SCORE << "BLUE::[" << SCORE_1 << "] <> RED::[" << SCORE_2 << "]";
        scoreText.setString(LIVE_SCORE.str());

        int TIME_LEFT = static_cast<int>(GAME_TIMER - USED_TIME);
        std::stringstream LIVE_TIME;
        LIVE_TIME << "TIME::[" << TIME_LEFT << "]";
        timerText.setString(LIVE_TIME.str());

        // Rendering
        window.clear();
        window.draw(backgroundSprite);
        window.draw(TOP_BORDER);
        window.draw(BOTTOM_BORDER);
        window.draw(LEFT_GOAL);
        window.draw(RIGHT_GOAL);

        window.draw(PAD_01_GLOW);
        window.draw(PAD_02_GLOW);
        window.draw(PAD_01);
        window.draw(PAD_02);

        for (const auto& t : ballTrail) {
            window.draw(t);
        }
        window.draw(BALL_GLOW);
        window.draw(BALL);
        window.draw(scoreText);
        window.draw(timerText);
        window.display();
    }

    return 0;
}

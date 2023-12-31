#include "Engine.h"
#include <iostream>
#include <cstdlib> 

#include "Engine.h"

Engine::Engine() 
{
    m_Window.create(VideoMode::getDesktopMode(), "Particles");
}

void Engine::run() {
    Clock clock;

    std::cout << "Starting Particle unit tests..." << std::endl;
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    std::cout << "Unit tests complete. Starting engine..." << std::endl;


    while (m_Window.isOpen()) {
        Time dt = clock.restart();

        float dtSeconds = dt.asSeconds();

        input();

        update(dtSeconds);

        draw();
    }
}


void Engine::input()
{
    Event event;
    while (m_Window.pollEvent(event)) 
    {
        if (event.type == Event::Closed || (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)) 
        {
            m_Window.close();
        }

        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) 
        {
            for (int i = 0; i < 5; ++i) 
            {
                int numPoints = rand() % 26 + 25;
                Vector2i mousePosition = Mouse::getPosition(m_Window);
                m_particles.push_back(Particle(m_Window, numPoints, mousePosition));
            }
        }
    }
}

void Engine::update(float dtAsSeconds) 
{
    for (auto it = m_particles.begin(); it != m_particles.end();) 
    {
        if (it->getTTL() > 0.0) 
        {
            it->update(dtAsSeconds);
            ++it;
        }
        else 
        {
            it = m_particles.erase(it);
        }
    }
}

void Engine::draw() 
{
    m_Window.clear();
    
    for (const auto& particle : m_particles) 
    {
        m_Window.draw(particle);
    }
    m_Window.display();
}

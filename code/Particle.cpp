#include "Particle.h"


bool Particle::almostEqual(double a, double b, double eps)
{
    return fabs(a - b) < eps;
}

void Particle::unitTests()
{
    int score = 0;

    cout << "Testing RotationMatrix constructor...";
    double theta = MPI / 4.0;
    RotationMatrix r(MPI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta)))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }


    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
    {
        cout << "Failed.  Expected (0,0).  Received: (" << m_centerCoordinate.x << "," << m_centerCoordinate.y << ")" << endl;
    }
    else
    {
        cout << "Passed.  +1" << endl;
        score++;
    }

    cout << "Applying one rotation of 90 degrees about the origin..." << endl;
    Matrix initialCoords = m_A;
    rotate(MPI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1, j), initialCoords(0, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j)) || !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            scalePassed = false;
        }
    }
    if (scalePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            translatePassed = false;
        }
    }
    if (translatePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Score: " << score << " / 7" << endl;
}

Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition)
    : m_A(2, numPoints)
{
    m_ttl = TTL;
    m_numPoints = numPoints;
    m_radiansPerSec = ((float)rand() / (RAND_MAX)) * MPI;
    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(target.getSize().x, (-1.0) * target.getSize().y);

    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    // for trail code
    m_pastPositions.push_back(m_centerCoordinate);

    m_vx = (rand() % 400) + 250;
    m_vy = (rand() & 400) + 250;
    
    // added to randomize direction (shoots either left or right)
    m_vx *= (rand() % 2 != 0) ? 1 : -1;

    m_color1 = Color::White;
    m_color2 = Color(rand() % 256, rand() % 256, rand() % 256);

    m_pulsatingAlpha = 240.0;

    float theta = ((float)rand() / RAND_MAX) * MPI / 2.0;
    float dTheta = ((2 * MPI) / (numPoints - 1));

    for (int i = 0; i < numPoints; i++)
    {
        float r = rand() % 61 + 20;
        float dx = r * cos(theta);
        float dy = r * sin(theta);

        m_A(0, i) = m_centerCoordinate.x + dx;
        m_A(1, i) = m_centerCoordinate.y + dy;
        
        theta += dTheta;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const
{
    // trail code part (justins part)
    for (size_t i = 1; i < m_pastPositions.size(); ++i) 
    {
    float alpha = static_cast<float>(i) / m_pastPositions.size(); 

    Color newColor = Color(
        static_cast<Uint8>(m_color1.r * (1.0 - alpha) + m_color2.r * alpha),
        static_cast<Uint8>(m_color1.g * (1.0 - alpha) + m_color2.g * alpha),
        static_cast<Uint8>(m_color1.b * (1.0 - alpha) + m_color2.b * alpha),
        static_cast<Uint8>(alpha * 255)
    );

    VertexArray lines(LineStrip, 2);
    lines[0].position = static_cast<Vector2f>(target.mapCoordsToPixel(m_pastPositions[i - 1], m_cartesianPlane));

    lines[0].color = newColor;
    lines[1].position = static_cast<Vector2f>(target.mapCoordsToPixel(m_pastPositions[i], m_cartesianPlane));

    lines[1].color = newColor;

    target.draw(lines, states);
}
    // trail code part (justins part)
    
    //start from here
    VertexArray lines(TriangleFan, m_numPoints + 1);
    Vector2f center = (Vector2f)(target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane));

    lines[0].position = center;
    lines[0].color = m_color1;  

    for (int j = 1; j <= m_numPoints; ++j)
    {
        lines[j].position = (Vector2f)(target.mapCoordsToPixel(Vector2f(m_A(0, j - 1), m_A(1, j - 1)), m_cartesianPlane));
        lines[j].color = m_color2;  
        lines[j].color.a = m_color2.a;
    }
    
    target.draw(lines, states);

}

void Particle::update(float dt)
{
    m_ttl -= dt;
    rotate(dt * m_radiansPerSec);
    scale(SCALE);
    float dx = m_vx * dt;
    m_vy -= G * dt;
    float dy = m_vy * dt;
    translate(dx, dy);
    transitionAlpha(dt);

    // this is for trail code (justins part)
    m_pastPositions.push_back(m_centerCoordinate);

    while (m_pastPositions.size() > 30) 
    {
        m_pastPositions.erase(m_pastPositions.begin());
    }
    // trail code
}

void Particle::transitionAlpha(float dt)
{
    const float PULSATING_ALPHA_TRANSITION_SPEED = 110.0;
    m_pulsatingAlpha += PULSATING_ALPHA_TRANSITION_SPEED * dt;
    m_color2.a = static_cast<Uint8>(std::sin(m_pulsatingAlpha * 0.01) * 240.0);
}

void Particle::translate(double xShift, double yShift)
{
    TranslationMatrix T(xShift, yShift, m_A.getCols());

    m_A = T + m_A;

    m_centerCoordinate.x += static_cast<float>(xShift);
    m_centerCoordinate.y += static_cast<float>(yShift);
}

void Particle::rotate(double theta)
{
    Vector2f temp = m_centerCoordinate;

    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);
    RotationMatrix R(theta);
    m_A = R * m_A;
    translate(temp.x, temp.y);
}

void Particle::scale(double c)
{
    Vector2f temp = m_centerCoordinate;
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);
    ScalingMatrix S(c);
    m_A = S * m_A;
    translate(temp.x, temp.y);
}

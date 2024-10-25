// Inellastic discrete

void Particle::handleWallCollisions(sf::Time deltaTime) //Optimizar
{
	r += v * deltaTime.asSeconds();
	v += a * deltaTime.asSeconds();

	if (r.x < 0 && v.x < 0 || r.x > conf::window_size_f.x - 2.f * conf::h && v.x > 0)
	{
		v.x *= -1.f;
	}
	if (r.y < 0 && v.y < 0 || r.y > conf::window_size_f.y - 2.f * conf::h && v.y > 0)
	{
		v.y *= -1.f;
	}
}

// Inellastic continuous

void Particle::handleWallCollisions(sf::Time deltaTime) //Optimizar
{
	sf::Vector2f next_v = v + a * deltaTime.asSeconds();
	sf::Vector2f next_r = r + v * deltaTime.asSeconds();

	bool horizontal = r.x > 0 && r.x < conf::window_size_f.x - 2.f * conf::h;
	bool vertical = r.y > 0 && r.y < conf::window_size_f.y - 2.f * conf::h;

	if (horizontal && vertical)
	{
		v = next_v;
		r = next_r;
	}
	else
	{
		if (!vertical)
		{
			//Fraction of time before collision
			const float gamma = (next_r.y < 0) ? -1.f * r.y / (conf::tau * v.y) : (conf::window_size_f.y - 2.f * conf::h - r.y) / (conf::tau * v.y);

			//Position through the wall, but with velocity correction
			next_r = r + gamma * conf::tau * v + (1.f - gamma) * conf::tau * conf::alpha * v;

			//Reflection
			next_r.y = (next_r.y < 0) ? -1.f * next_r.y : 2.f * (conf::window_size_f.y - 2.f * conf::h) - next_r.y;
			next_v.y *= -1.f * conf::alpha;

			//Assignment
			r = next_r;
			v = next_v;
		}
		if (!horizontal) //Implicity handles the case when !(horizontal&&vertical)
		{
			//Fraction of time before collision
			const float gamma = (next_r.x < 0) ? -1.f * r.x / (conf::tau * v.x) : (conf::window_size_f.x - 2.f * conf::h - r.x) / (conf::tau * v.x);

			//Position through the wall, but with velocity correction
			next_r = r + gamma * conf::tau * v + (1.f - gamma) * conf::tau * conf::alpha * v;

			//Reflection
			next_r.x = (next_r.x < 0) ? -1.f * next_r.x : 2.f * (conf::window_size_f.x - 2.f * conf::h) - next_r.x;
			next_v.x *= -1.f * conf::alpha;

			//Assignment
			r = next_r;
			v = next_v;
		}
	}
}
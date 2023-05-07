#include<Aurora/Utils/Color.h>

int Color::toInt() const
{
	return ((int)(255 * a) << 24) | ((int)(255 * b) << 16) | ((int)(255 * g) << 8) | ((int)(255 * r));
}

Color Color::random()
{
	return { Random::Float(),Random::Float() ,Random::Float(),1.f };
}

Color Color::HSVtoRGB(const Color& c)
{
	float r, g, b, f, p, q, t;
	const int i = (int)floorf(c.r * 6.f);
	f = c.r * 6 - i;
	p = c.b * (1 - c.g);
	q = c.b * (1 - f * c.g);
	t = c.b * (1 - (1 - f) * c.g);

	switch (i % 6) {
	default:
	case 0: r = c.b, g = t, b = p; break;
	case 1: r = q, g = c.b, b = p; break;
	case 2: r = p, g = c.b, b = t; break;
	case 3: r = p, g = q, b = c.b; break;
	case 4: r = t, g = p, b = c.b; break;
	case 5: r = c.b, g = p, b = q; break;
	}

	return { r,g,b,1.f };
}

bool Color::operator==(const Color& color) const
{
	return toInt() == color.toInt();
}

bool Color::operator!=(const Color& color) const
{
	return toInt() != color.toInt();
}

Color::operator const float* () const
{
	return (const float*)this;
}

const Color Color::Gold{ 1.f,0.843f,0.f,1.f };

const Color Color::White{ 1.f,1.f,1.f,1.f };

const Color Color::Invisible{ 0.f,0.f,0.f,0.f };

const Color Color::Black{ 0.f,0.f,0.f,1.f };
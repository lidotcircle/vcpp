#ifndef SM_ENTITY_HPP
#define SM_ENTITY_HPP

#include<vector>
#include<cmath>

#include<d2d1.h>

template<typename T>
inline void SafeReleaseX(T** ptr) {
	if (*ptr) {
		(*ptr)->Release();
		*ptr = NULL;
	}
}

class D_Drawable_d2 
{
public:
	virtual bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) = 0;
};

// object of beam, node, support, force, reaction
template<typename T>
class D_RGBColor {
public:
	typedef T value_type;

private:
	typedef D2D1::ColorF d2d1color;
	value_type m_a;
	value_type m_r;
	value_type m_g;
	value_type m_b;

public:
	constexpr D_RGBColor(value_type r, value_type g = r, value_type b = r, value_type a = 1) :
		m_a(a), m_r(r), m_g(g), m_b(b) {}
	inline value_type& GetAlpha() { return this->m_a; }
	inline value_type& GetR() { return this->m_r; }
	inline value_type& GetG() { return this->m_g; }
	inline value_type& GetB() { return this->m_b; }

	static D_RGBColor red, green, blue, white, black;

	operator d2d1color() {
		d2d1color ret = d2d1color(m_r, m_g, m_b, m_a);
		return ret;
	}
};
template<typename T>
D_RGBColor<T> D_RGBColor<T>::red   = D_RGBColor(255, 0, 0, 1);
template<typename T>
D_RGBColor<T> D_RGBColor<T>::green = D_RGBColor(0, 255, 0, 1);
template<typename T>
D_RGBColor<T> D_RGBColor<T>::blue  = D_RGBColor(0, 0, 255, 1);
template<typename T>
D_RGBColor<T> D_RGBColor<T>::white = D_RGBColor(255, 255, 255, 1);
template<typename T>
D_RGBColor<T> D_RGBColor<T>::black = D_RGBColor(0, 0, 0, 1);

template<typename T>
class D_Node: D_Drawable_d2 
{
public:
	typedef T value_type;
	typedef size_t node_id;
private:
	typedef decltype(D2D1::Point2F()) d2d1point2f;
	value_type m_x;
	value_type m_y;
public:
	D_Node<value_type>(value_type x, value_type y) : m_x(x), m_y(y) {}

	value_type& GetX() { return m_x; }
	value_type& GetY() { return m_y; }

	bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) {
		if (target == nullptr || factory == nullptr) { return false; }
		auto ellipse = D2D1::Ellipse(D2D1::Point2F(m_x, m_y), 2.0, 2.0);
		ID2D1EllipseGeometry* ellipseGeo = nullptr;
		ID2D1SolidColorBrush* brush = nullptr;
		if (!SUCCEEDED(target->CreateSolidColorBrush(D2D1::ColorF(0.2, 0.2, 0.2), &brush))) return false;
		if (!SUCCEEDED(factory->CreateEllipseGeometry(ellipse, &ellipseGeo))) { SafeReleaseX(&brush); return false; }
		target->DrawGeometry(ellipseGeo, brush);
		SafeReleaseX(&ellipseGeo);
		SafeReleaseX(&brush);
	}
	operator d2d1point2f() const
	{
		return D2D1::Point2F(this->m_x, this->m_y);
	}
};

template<typename T>
class D_Beam {
public:
	typedef T value_type;

private:
	D_Node<value_type>& m_start;
	D_Node<value_type>& m_end;
	D_RGBColor<value_type> m_color;

public:
	D_Beam(D_Node<value_type>& s, D_Node<value_type>& e, D_RGBColor<value_type> color = D_RGBColor<value_type>::black) :
		m_start(s), m_end(e), m_color(color) {}

	bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) {
		if (target == nullptr || factory == nullptr) { return false; }
		ID2D1SolidColorBrush* brush = nullptr;
		if (!SUCCEEDED(target->CreateSolidColorBrush(this->m_color, &brush))) return false;
		target->DrawLine(this->m_start, this->m_end, brush);
		SafeReleaseX(&brush);
		return true;
	}

};

template<typename T>
class D_Support;
template<typename T>
class D_LineSegment
{
public:
	typedef T value_type;
private:
	friend class D_Support<value_type>;
	value_type             m_dx;
	value_type             m_dy;
	value_type             m_x_off;
	value_type             m_y_off;
	D_RGBColor<value_type> m_color;
	value_type             m_bold;
public:
	D_LineSegment(value_type dx, value_type dy,  value_type x_off = 0, value_type y_off = 0, value_type bold = 1, D_RGBColor<value_type> color = D_RGBColor<value_type>::black):
		 m_dx(dx), m_dy(dy), m_color(color), m_bold(bold), m_x_off(x_off), m_y_off(y_off){}

	void ScaleBy(value_type scale) { this->m_dx *= scale; this->m_dy *= scale; this->m_x_off *= scale; this->m_y_off *= scale; }

	static D_LineSegment CreateLineSegment(value_type dx, value_type dy, D_Node<value_type>& origin, D_RGBColor<value_type> color = D_RGBColor<value_type>::black)
	{
		D_LineSegment ret(dx, dy, origin, color);
		return ret;
	}
	static D_LineSegment CreateLineSegmentPolar(value_type theta, value_type rho, D_Node<value_type>& origin, D_RGBColor<value_type> color = D_RGBColor<value_type>::black)
	{
		D_LineSegment ret(::cos(theta) * rho, ::sin(theta) * rho, origin, color);
		return ret;
	}
};

template<typename T>
class D_Support
{
public:
	typedef T value_type;

private:
	D_Node<value_type>& m_node;
	std::vector<D_LineSegment<value_type>> m_lines;

	D_Support(D_Node<value_type>& node): m_node(node), m_lines(){}

public:
	D_Support() = delete;
	std::vector<D_LineSegment<value_type>>& LineSegment() { return this->m_lines; }

	static D_Support CreateHingeSupport(D_Node<value_type>& node) {
		D_Support ret(node);
		ret.m_lines.push_back(D_LineSegment<value_type>(0.3, -0.7, -0.3, -0.7, 1.5));
		ret.m_lines.push_back(D_LineSegment<value_type>(0.3, -0.7, 0.0, 0.0, 1.5));
		ret.m_lines.push_back(D_LineSegment<value_type>(-0.3, -0.7, 0.0, 0.0, 1.5));
		for (auto bi = ret.m_lines.begin(); bi != ret.m_lines.end(); bi++)
			bi->ScaleBy(10);
		return ret;
	}

	bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) {
		if (target == nullptr || factory == nullptr) { return false; }
		ID2D1SolidColorBrush* brush = nullptr;
		for (auto bi = this->m_lines.begin(); bi != this->m_lines.end(); bi++) {
			if (!SUCCEEDED(target->CreateSolidColorBrush(bi->m_color, &brush))) return false;
			auto p1 = D2D1::Point2F(this->m_node.GetX() + bi->m_x_off, this->m_node.GetY() + bi->m_y_off);
			auto p2 = D2D1::Point2F(this->m_node.GetX() + bi->m_dx, this->m_node.GetY() + bi->m_dy);
			target->DrawLine(p1, p2, brush, bi->m_bold);
			SafeReleaseX(&brush);
		}
		return true;
	}
};

template class D_Node<double>;
template class D_RGBColor<double>;
template class D_Beam<double>;
template class D_Support<double>;

#endif //SM_ENTITY_HPP
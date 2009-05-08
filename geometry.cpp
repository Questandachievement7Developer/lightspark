/**************************************************************************
    Lighspark, a free flash player implementation

    Copyright (C) 2009  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include <fstream>
#include <math.h>
#include <GL/gl.h>
#include "swftypes.h"
#include "logger.h"
#include "geometry.h"

using namespace std;

float colors[][3] = { { 0 ,0 ,0 },
			{1, 0, 0},
			{0, 1, 0},
			{0 ,0, 1},
			{1 ,1, 0},
			{1,0 ,1},
			{0,1,1},
			{0,0,0}};

void Shape::Render(int i) const
{
	if(graphic.filled0 && graphic.filled1)
	{
		LOG(NOT_IMPLEMENTED,"Not supported double fill style");
		//Shape* th=const_cast<Shape*>(this);
		//th->graphic.color0=RGB(255,0,0);
		//th->graphic.color1=RGB(0,0,255);
	}

	if(winding==0)
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glStencilFunc(GL_ALWAYS,0,0);
		if(graphic.filled0)
			glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
		else
			glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
		std::vector<Triangle>::const_iterator it2=interior.begin();
		glBegin(GL_TRIANGLES);
		for(it2;it2!=interior.end();it2++)
		{
			glVertex2i(it2->v1.x,it2->v1.y);
			glVertex2i(it2->v2.x,it2->v2.y);
			glVertex2i(it2->v3.x,it2->v3.y);
		}
		glEnd();
	}
	else if(winding==1)
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glStencilFunc(GL_ALWAYS,0,0);
		if(graphic.filled1)
			glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
		else
			glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
		std::vector<Triangle>::const_iterator it2=interior.begin();
		glBegin(GL_TRIANGLES);
		for(it2;it2!=interior.end();it2++)
		{
			glVertex2i(it2->v1.x,it2->v1.y);
			glVertex2i(it2->v2.x,it2->v2.y);
			glVertex2i(it2->v3.x,it2->v3.y);
		}
		glEnd();
	}


//	if(graphic.stroked)
	{
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_NEVER,0,0);
		std::vector<Vector2>::const_iterator it=outline.begin();
		//glColor3ub(graphic.stroke_color.Red,graphic.stroke_color.Green,graphic.stroke_color.Blue);
		glColor3f(colors[i%8][0],colors[i%8][1],colors[i%8][2]);
		if(closed)
			glBegin(GL_LINE_LOOP);
		else
			glBegin(GL_LINE_STRIP);
		for(it;it!=outline.end();it++)
		{
			glVertex2i(it->x,it->y);
		}
		glEnd();
		glEnable(GL_STENCIL_TEST);
	}

	for(int i=0;i<sub_shapes.size();i++)
		sub_shapes[i].Render();
}

/*bool Edge::xIntersect(int x,int32_t& d)
{
	int u1,u2;
	u1=min(x1,x2);
	u2=max(x1,x2);
	if((x>=u1) && (x<=u2))
	{
		if(x2==x1)
			d=y1;
		else
		{
			float m=(y2-y1);
			m/=(x2-x1);
			d=m*(x-x1);
			d+=y1;
		}
		return true;
	}
	else
		return false;
}*/

bool Edge::yIntersect(const Vector2& p, int& x)
{
	if(p.y<p1.y || p.y>=p2.y)
		return false;
	else
	{
		Vector2 c=p2-p1;
		Vector2 d=p-p1;

		x=(c.x*d.y)/c.y+p1.x;
		return true;
	}
}

/*bool Edge::edgeIntersect(const Edge& e)
{
	float ua=(e.x2-e.x1)*(y1-e.y1)-(e.y2-e.y1)*(x1-e.x1);
	float ub=(x2-x1)*(y1-e.y1)-(y2-y1)*(x1-e.x1);

	ua/=(e.y2-e.y1)*(x2-x1)-(e.x2-e.x1)*(y2-y1);
	ub/=(e.y2-e.y1)*(x2-x1)-(e.x2-e.x1)*(y2-y1);

	if(ua>0 && ua<1 && ub>0 && ub<1)
		return true;
	else
		return false;
}*/

FilterIterator::FilterIterator(const std::vector<Vector2>::const_iterator& i, const std::vector<Vector2>::const_iterator& e, int f):it(i),end(e),filter(f)
{
	if(it!=end && it->index==filter)
		it++;
}
FilterIterator FilterIterator::operator++(int)
{
	FilterIterator copy(*this);
	it++;
	if(it!=end && it->index==filter)
		it++;
	return copy;
}

const Vector2& FilterIterator::operator*()
{
	return *it;
}

bool FilterIterator::operator==(FilterIterator& i)
{
	return i.it==it;
}

bool FilterIterator::operator!=(FilterIterator& i)
{
	return i.it!=it;
}

void Shape::dumpEdges()
{
		ofstream f("edges.dat");

		for(int i=0;i<edges.size();i++)
			f << edges[i].p1.x << ' ' << edges[i].p1.y << endl;
/*			for(int k=0;k<cached[j].sub_shapes.size();k++)
		{
			for(int i=0;i<cached[j].sub_shapes[k].edges.size();i++)
				f << cached[j].sub_shapes[k].edges[i].p1.x << ' ' << cached[j].sub_shapes[k].edges[i].p1.y << endl;
		}*/
		f.close();
}

void Shape::BuildFromEdges()
{
	for(int i=0;i<edges.size();i++)
		outline.push_back(edges[i].p1);
}

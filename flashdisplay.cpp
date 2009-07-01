/**************************************************************************
    Lightspark, a free flash player implementation

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

#include <list>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "flashdisplay.h"
#include "swf.h"

using namespace std;

extern __thread SystemState* sys;

ASFUNCTIONBODY(LoaderInfo,_constructor)
{
//	setVariableByName("parameters",&parameters);
}

ASFUNCTIONBODY(Loader,_constructor)
{
	ISWFObject* ret=obj->setVariableByName("contentLoaderInfo",new LoaderInfo);
	ret->bind();
	ret->constructor->call(ret,NULL);
}

MovieClip::MovieClip():_visible(1),_x(0),_y(0),_height(100),_width(100),_framesloaded(0),_totalframes(1),
	displayListLimit(0),rotation(0.0)
{
	sem_init(&sem_frames,0,1);
	class_name="MovieClip";
	MovieClip::_register();
}

bool MovieClip::list_orderer(const IDisplayListElem* a, int d)
{
	return a->getDepth()<d;
}

void MovieClip::addToDisplayList(IDisplayListElem* t)
{
	list<IDisplayListElem*>::iterator it=lower_bound(displayList.begin(),displayList.end(),t->getDepth(),list_orderer);
	displayList.insert(it,t);
	displayListLimit=displayList.size();
}

ASFUNCTIONBODY(MovieClip,createEmptyMovieClip)
{
	LOG(NOT_IMPLEMENTED,"createEmptyMovieClip");
	return new Undefined;
/*	MovieClip* th=dynamic_cast<MovieClip*>(obj);
	if(th==NULL)
		LOG(ERROR,"Not a valid MovieClip");

	LOG(CALLS,"Called createEmptyMovieClip: " << args->args[0]->toString() << " " << args->args[1]->toString());
	MovieClip* ret=new MovieClip();

	IDisplayListElem* t=new ASObjectWrapper(ret,args->args[1]->toInt());
	list<IDisplayListElem*>::iterator it=lower_bound(th->dynamicDisplayList.begin(),th->dynamicDisplayList.end(),t->getDepth(),list_orderer);
	th->dynamicDisplayList.insert(it,t);

	th->setVariableByName(args->args[0]->toString(),ret);
	return ret;*/
}

ASFUNCTIONBODY(MovieClip,moveTo)
{
	LOG(NOT_IMPLEMENTED,"Called moveTo");
	return NULL;
}

ASFUNCTIONBODY(MovieClip,lineTo)
{
	LOG(NOT_IMPLEMENTED,"Called lineTo");
	return NULL;
}

ASFUNCTIONBODY(MovieClip,lineStyle)
{
	LOG(NOT_IMPLEMENTED,"Called lineStyle");
	return NULL;
}

ASFUNCTIONBODY(MovieClip,swapDepths)
{
	LOG(NOT_IMPLEMENTED,"Called swapDepths");
	return NULL;
}

void MovieClip::_register()
{
	setVariableByName("_visible",&_visible);
	setVariableByName("y",&_y);
	setVariableByName("x",&_x);
	setVariableByName("width",&_width);
	rotation.bind();
	setVariableByName("rotation",&rotation,true);
	setVariableByName("height",&_height);
	setVariableByName("_framesloaded",&_framesloaded);
	setVariableByName("_totalframes",&_totalframes);
	setVariableByName("swapDepths",new Function(swapDepths));
	setVariableByName("lineStyle",new Function(lineStyle));
	setVariableByName("lineTo",new Function(lineTo));
	setVariableByName("moveTo",new Function(moveTo));
	setVariableByName("createEmptyMovieClip",new Function(createEmptyMovieClip));
	setVariableByName("addEventListener",new Function(addEventListener));
}

void MovieClip::Render()
{
	LOG(TRACE,"Render MovieClip");
	parent=sys->currentClip;
	MovieClip* clip_bak=sys->currentClip;
	sys->currentClip=this;

	if(!state.stop_FP && class_name=="MovieClip")
		state.next_FP=min(state.FP+1,frames.size()-1);
	else
		state.next_FP=state.FP;

	list<Frame>::iterator frame=frames.begin();

	//Set the id in the secondary color
	glPushAttrib(GL_CURRENT_BIT);
	glSecondaryColor3f(id,0,0);
	//Apply local transformation
	glPushMatrix();
	//glTranslatef(_x,_y,0);
	glRotatef(rotation,0,0,1);
	frame->Render(displayListLimit);

	glPopMatrix();
	glPopAttrib();

	//Render objects added at runtime;
	list<IDisplayListElem*>::iterator it=dynamicDisplayList.begin();
	for(it;it!=dynamicDisplayList.end();it++)
		(*it)->Render();

	if(state.FP!=state.next_FP)
	{
		state.FP=state.next_FP;
		sys->setUpdateRequest(true);
	}
	LOG(TRACE,"End Render MovieClip");

	sys->currentClip=clip_bak;
}


#include "OSDLWorldObject.h"

using std::string ;


using namespace OSDL::Engine ;



WorldObject::WorldObject() throw() : Ceylan::Model()
{

}


WorldObject::~WorldObject() throw()
{

}


const string WorldObject::toString( Ceylan::VerbosityLevels level ) const throw()
{	
	return "World object. " + Ceylan::Model::toString( level ) ;
}

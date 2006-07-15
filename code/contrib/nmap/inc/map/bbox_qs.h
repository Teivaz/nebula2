#ifndef N_BBOX_QS_H
#define N_BBOX_QS_H
//-------------------------------------------------------------------
/**
    @class bbox3_qs
    @ingroup NMapContribModule
    @brief (non-oriented) bounding box streamlined for 	quadtree navigation
    by caching up heavily used data
*/
//-------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

//-------------------------------------------------------------------
//  bbox3_qs
//-------------------------------------------------------------------
class bbox3_qs : public bbox3 {
public:
	float xm, ym, zm;
	float dxhalf, dyhalf, dzhalf;

    //--- constructors ----------------------------------------------
    bbox3_qs() {};
    bbox3_qs(const vector3& _vmin, const vector3& _vmax){
		bbox3::bbox3( _vmin, _vmax); 
		reset_qs();
	};
    bbox3_qs(const bbox3& bb){ 
		bbox3::bbox3( bb.vmin, bb.vmax );
		reset_qs();
	};
    
    //--- initialize from vector3 cloud -----------------------------
    bbox3_qs(vector3 *varray, int num) {
        vmin = varray[0];
        vmax = varray[0];
        int i;
        for (i=0; i<num; i++) {
            if      (varray[i].x<vmin.x) vmin.x=varray[i].x;
            else if (varray[i].x>vmax.x) vmax.x=varray[i].x;
            if      (varray[i].y<vmin.y) vmin.y=varray[i].y;
            else if (varray[i].y>vmax.y) vmax.y=varray[i].y;
            if      (varray[i].z<vmin.z) vmin.z=varray[i].z;
            else if (varray[i].z>vmax.z) vmax.z=varray[i].z;
        }
		reset_qs();
    };

	void reset_qs()
	{
        xm = ((vmin.x+vmax.x)*0.5f);
        ym = ((vmin.y+vmax.y)*0.5f);
        zm = ((vmin.z+vmax.z)*0.5f);
        dxhalf = (float) fabs(vmax.x-vmin.x)*0.5f;
        dyhalf = (float) fabs(vmax.y-vmin.y)*0.5f;
        dzhalf = (float) fabs(vmax.z-vmin.z)*0.5f;
	}

    //--- setting elements ------------------------------------------
    void set(const vector3& _vmin, const vector3& _vmax) {
        vmin = _vmin;
        vmax = _vmax;
		
		reset_qs();
    };

    void set(vector3 *varray, int num) {
        vmin = varray[0];
        vmax = varray[0];
        int i;
        for (i=0; i<num; i++) {
            if      (varray[i].x<vmin.x) vmin.x=varray[i].x;
            else if (varray[i].x>vmax.x) vmax.x=varray[i].x;
            if      (varray[i].y<vmin.y) vmin.y=varray[i].y;
            else if (varray[i].y>vmax.y) vmax.y=varray[i].y;
            if      (varray[i].z<vmin.z) vmin.z=varray[i].z;
            else if (varray[i].z>vmax.z) vmax.z=varray[i].z;
        }
		reset_qs();
    };

    //--- invalidate bounding box to prepare for growing ------------
    void begin_grow(void) {
        vmin.set(+1000000.0f,+1000000.0f,+1000000.0f);
        vmax.set(-1000000.0f,-1000000.0f,-1000000.0f);
    };
    void grow(const vector3& v) {
        if (v.x<vmin.x) vmin.x=v.x;
        if (v.x>vmax.x) vmax.x=v.x;
        if (v.y<vmin.y) vmin.y=v.y;
        if (v.y>vmax.y) vmax.y=v.y;
        if (v.z<vmin.z) vmin.z=v.z;
        if (v.z>vmax.z) vmax.z=v.z;
		reset_qs();
    };
    void grow(vector3 *varray, int num) {
        int i;
        for (i=0; i<num; i++) {
            grow(varray[i]);
        }
    };

    void grow(bbox3_qs *bb) {
        if (bb->vmin.x<vmin.x) vmin.x=bb->vmin.x;
        if (bb->vmin.y<vmin.y) vmin.y=bb->vmin.y;
        if (bb->vmin.z<vmin.z) vmin.z=bb->vmin.z;
        if (bb->vmax.x>vmax.x) vmax.x=bb->vmax.x;
        if (bb->vmax.y>vmax.y) vmax.y=bb->vmax.y;
        if (bb->vmax.z>vmax.z) vmax.z=bb->vmax.z;
		reset_qs();
    };
};
#endif


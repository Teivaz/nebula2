#include "export/nmaxexport.h"
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
*/

//------------------------------------------------------------------------------
/**
*/
Matrix3
nMaxExport::GetNodeTM(IGameNode *igNode, int time, int type)
{
	// initialize matrix with the identity
	Matrix3 tm;
	tm.IdentityMatrix();

	// only do this for valid nodes
	if(igNode != 0)
	{
		// get the node transformation
        switch (type)
        {
        case 0:
            tm = igNode->GetMaxNode()->GetNodeTM(time);
            break;
        case 1:
            tm = igNode->GetLocalTM(time).ExtractMatrix3();
            break;
        case 2:
            tm = igNode->GetWorldTM(time).ExtractMatrix3();
            break;
        }

		// make the transformation uniform
		tm.NoScale();
	}

	return tm;
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxExport::GetPRS(IGameNode *igNode, int time, vector4& pos, quaternion& rot, vector4& scale, int type)
{
    Matrix3 m = this->GetNodeTM(igNode, time, type);

    Point3 p = m.GetTrans();
    pos.x = p[0];
    pos.y = p[1];
    pos.z = p[2];
    pos.w = 0.0;

    Quat q(m);
    q.Invert();
    rot.x = q[0];
    rot.y = q[1];
    rot.z = q[2];
    rot.w = q[3];

    // !!! check this
    scale.x = 1.0;
    scale.y = 1.0;
    scale.z = 1.0;
    scale.w = 1.0;
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxExport::GetPRSBoneSpace(IGameNode *igNode, int time, vector4& pos, quaternion& rot, vector4& scale, int type)
{
    Matrix3 m;
    if (igNode->GetNodeParent())
    {
        m = this->GetNodeTM(igNode, time, type) * Inverse(this->GetNodeTM(igNode->GetNodeParent(), time, type));
    }
    else
    {
        m = Inverse(this->GetNodeTM(igNode, time, type));
    }

    Point3 p = m.GetTrans();
    pos.x = p[0];
    pos.y = p[1];
    pos.z = p[2];
    pos.w = 0.0;

    Quat q(m);
    rot.x = q[0];
    rot.y = q[1];
    rot.z = q[2];
    rot.w = q[3];

    // !!! check this
    scale.x = 1.0;
    scale.y = 1.0;
    scale.z = 1.0;
    scale.w = 1.0;
}

//------------------------------------------------------------------------------
/**
    build the matrix to transform the object from worldspace back to modelspace
*/
matrix44*
nMaxExport::buildInverseModelWorldMatrix(IGameNode* igNode)
{
    Matrix3 ObjectTM = igNode->GetWorldTM().ExtractMatrix3();
    
    AffineParts parts;
    decomp_affine(ObjectTM, &parts);//!!!expensive

    matrix44* m = n_new matrix44;

    m->scale(this->ScaleComponent(parts, igNode->GetName()));
    m->mult_simple(matrix44(this->RotationComponent(parts)));
    m->translate(this->PositionComponent(parts));
    m->invert();

    return m;
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxExport::PositionComponent(Matrix3 m)
{
    AffineParts parts;
    decomp_affine(m, &parts);//!!!expensive
    return this->PositionComponent(parts);
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxExport::PositionComponent(const AffineParts &parts)
{
    vector3 position(parts.t.x, parts.t.y, parts.t.z);
    return position;
}

//------------------------------------------------------------------------------
/**
*/
quaternion
nMaxExport::RotationComponent(Matrix3 m)
{
    AffineParts parts;
    decomp_affine(m, &parts);//!!!expensive
    return this->RotationComponent(parts);
}

//------------------------------------------------------------------------------
/**
*/
quaternion
nMaxExport::RotationComponent(const AffineParts &parts)
{
    quaternion q(parts.q.x, parts.q.y, parts.q.z, parts.q.w);
    return q;
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxExport::ScaleComponent(Matrix3 m, const TCHAR* nodeName)
{
    AffineParts parts;
    decomp_affine(m, &parts);//!!!expensive
    return this->ScaleComponent(parts.u, parts.k, nodeName);
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxExport::ScaleComponent(const AffineParts &parts, const TCHAR* nodeName)
{
    return this->ScaleComponent(parts.u, parts.k, nodeName);
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxExport::ScaleComponent(const Quat &u, const Point3 &k, const TCHAR* nodeName)
{   
    //check if the scale axis system is not equal to the nebula axis system
    if (u.IsIdentity() == 0)
    {
        //convert the scale from max scale axis to nebula scale axis system       
        n_printf("WARNING: '%s': There is a nonuniform scale, this may result in unecpected data!.\n", nodeName ? nodeName : "UNKNOWN NODE");        

        Matrix3 scaleAxis;
        u.MakeMatrix(scaleAxis, false);

        //the inverse target axis system
        Matrix3 inverseIdent(true);
        inverseIdent.Invert();

        //transform the max scale value (RH axis to LH axis)
        Point3 scaleInIdent = (scaleAxis * inverseIdent) * k;

        //flip the values (needed because of transform from RH/LH)

        //FIXME: clear if the values for animations must be multiplied  with -1.0f
        vector3 scale(-1.0f * scaleInIdent.x, -1.0f * scaleInIdent.y, -1.0f * scaleInIdent.z);
        //vector3 scale(scaleInIdent.x, scaleInIdent.y, scaleInIdent.z);
        return scale;
    }

    return vector3(k.x, k.y, k.z);
}

//------------------------------------------------------------------------------
/**
    Export the position of the Max Node, create a animator if needed.
*/
void
nMaxExport::exportPosition(nTransformNode* nNode, nString nodeName, IGameNode* igNode)
{
    n_assert(nNode);
    n_assert(igNode);
    n_assert(this->task);

    IGameControl* igControl = igNode->GetIGameControl();
    if (igControl)
    {
        //get position keys
        IGameKeyTab posKeys;

        //check position control type
        switch (igControl->GetControlType(IGAME_POS))
        {
            case IGameControl::IGAME_UNKNOWN:
            case IGameControl::IGAME_MAXSTD:
            case IGameControl::IGAME_BIPED:
                {
                    //try to get the keys
                    if (!igControl->GetQuickSampledKeys(posKeys, IGAME_POS))
                    {   //if it's not possible to get keys sample the data
                        igControl->GetFullSampledKeys(posKeys, this->task->sampleRate, IGAME_POS);
                    }
                }
                break;    
            case IGameControl::IGAME_POS_CONSTRAINT:
                n_printf("FIXME: Position Constraint control not supported!\n");
                break;
            case IGameControl::IGAME_LINK_CONSTRAINT:
                n_printf("FIXME: Link Constraint control not supported!\n");
                break;
            case IGameControl::IGAME_EULER:
            case IGameControl::IGAME_ROT_CONSTRAINT:
                n_error("ERROR: Wrong Control type!\n");
        }
        
        //write position
        if (posKeys.Count() > 1 && this->task->exportAnimations)
        {
            //create positon animator
            nString anim(nodeName);
            anim += "/posAnim";
            nTransformAnimator* nAnim = static_cast<nTransformAnimator*>(nKernelServer::Instance()->New("ntransformanimator", anim.Get()));
            
            for (int i = 0; i < posKeys.Count(); i++)
            {
                vector3 key(posKeys[i].sampleKey.pval.x, posKeys[i].sampleKey.pval.y, posKeys[i].sampleKey.pval.z);
                nAnim->AddPosKey(this->tickTime * posKeys[i].t, key);
            }
            nAnim->SetChannel("time");

            nNode->AddAnimator("posAnim");
        }
        else
        {
            nNode->SetPosition(this->PositionComponent(igNode->GetLocalTM().ExtractMatrix3()));
        }
    }
    else
    {
        nNode->SetPosition(this->PositionComponent(igNode->GetLocalTM().ExtractMatrix3()));
    }
}   
        

//------------------------------------------------------------------------------
/**
    Export the rotation of the max node, create a animator if needed.

    @TODO add direct euler keys export with IGame v1.122
*/
void
nMaxExport::exportRotation(nTransformNode* nNode, nString nodeName, IGameNode* igNode)
{
    IGameControl* igControl = igNode->GetIGameControl();
    if (igControl)
    {
        switch (igControl->GetControlType(IGAME_ROT))
        {
            case IGameControl::IGAME_EULER:
                //FIXME: add EulerKey export with IGame v1.122
            case IGameControl::IGAME_UNKNOWN:
            case IGameControl::IGAME_MAXSTD:
            case IGameControl::IGAME_BIPED:
                {
                    IGameKeyTab keys;

                    if ( ! igControl->GetQuickSampledKeys(keys, IGAME_ROT) )
                    {
                        igControl->GetFullSampledKeys(keys, this->task->sampleRate, IGAME_ROT);
                    }

                    if (keys.Count() > 1 && this->task->exportAnimations)
                    {
                        //create animator
                        nString anim(nodeName);
                        anim += "/rotAnim";
                        nTransformAnimator* nAnim = static_cast<nTransformAnimator*>(nKernelServer::Instance()->New("ntransformanimator", anim.Get()));

                        for (int i = 0; i < keys.Count(); i++)
                        {
                            quaternion q(keys[i].sampleKey.qval.x, keys[i].sampleKey.qval.y, keys[i].sampleKey.qval.z, keys[i].sampleKey.qval.w);
                            nAnim->AddQuatKey(this->tickTime * keys[i].t, q);
                        }
                        nAnim->SetChannel("time");

                        nNode->AddAnimator("rotAnim");
                    }
                    else
                    {
                        //get rotation from matrix
                        nNode->SetQuat(this->RotationComponent(igNode->GetLocalTM().ExtractMatrix3()));
                    }
                }
                break;
            case IGameControl::IGAME_ROT_CONSTRAINT:
                n_printf("FIXME: Rotation Constraint control not supported!\n");
                break;
            case IGameControl::IGAME_LINK_CONSTRAINT:
                n_printf("FIXME: Link Constraint control not supported!\n");
                break;
            case IGameControl::IGAME_POS_CONSTRAINT:
                n_error("ERROR: Wrong Contol type!\n");
        }

    }
    else
    {
        nNode->SetQuat(this->RotationComponent(igNode->GetLocalTM().ExtractMatrix3()));
    }
}

//------------------------------------------------------------------------------
/**
    Export the scale of the Max Node, create a animator if needed.
*/
void
nMaxExport::exportScale(nTransformNode* nNode, nString nodeName, IGameNode* igNode)
{
    IGameControl* igControl = igNode->GetIGameControl();
    if (igControl)
    {
        //scale keys
        IGameKeyTab scaleKeys;
        switch (igControl->GetControlType(IGAME_SCALE))
        {
            case IGameControl::IGAME_UNKNOWN:
            case IGameControl::IGAME_MAXSTD:
            case IGameControl::IGAME_BIPED:
                {
                    if (!igControl->GetQuickSampledKeys(scaleKeys, IGAME_SCALE))
                    {
                        igControl->GetFullSampledKeys(scaleKeys, this->task->sampleRate, IGAME_SCALE);
                    }
                }
                break;
            case IGameControl::IGAME_LINK_CONSTRAINT:
                n_printf("ERROR: Link Constraint control not supported!\n");
                break;
            case IGameControl::IGAME_POS_CONSTRAINT:
            case IGameControl::IGAME_ROT_CONSTRAINT:
            case IGameControl::IGAME_EULER:
                n_error("ERROR: Wrong Contol type!\n");
        }

        if (scaleKeys.Count() > 1 && this->task->exportAnimations)
        {
            //create animator
            nString anim(nodeName);
            anim += "/scaleAnim";
            nTransformAnimator* nAnim = static_cast<nTransformAnimator*>(nKernelServer::Instance()->New("ntransformanimator", anim.Get()));
            
            for (int i = 0; i < scaleKeys.Count(); i++)
            {
                nAnim->AddScaleKey(this->tickTime * scaleKeys[i].t, this->ScaleComponent(scaleKeys[i].sampleKey.sval.q, scaleKeys[i].sampleKey.sval.s, igNode->GetName()));
            }
            nAnim->SetChannel("time");

            nNode->AddAnimator("scaleAnim");
        }
        else
        {
            nNode->SetScale(this->ScaleComponent(igNode->GetLocalTM().ExtractMatrix3(), igNode->GetName()));
        }
    }
    else
    {
        nNode->SetScale(this->ScaleComponent(igNode->GetLocalTM().ExtractMatrix3(), igNode->GetName()));
    }
}


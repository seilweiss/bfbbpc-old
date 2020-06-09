#include "xMath3.h"

#include "iMath3.h"
#include "iMath.h"
#include "xMathInlines.h"

xVec3 g_O3 = { 0.0f, 0.0f, 0.0f };

xVec3 g_X3 = { 1.0f, 0.0f, 0.0f };
xVec3 g_Y3 = { 0.0f, 1.0f, 0.0f };
xVec3 g_Z3 = { 0.0f, 0.0f, 1.0f };

xVec3 g_NX3 = { -1.0f, 0.0f, 0.0f };
xVec3 g_NY3 = { 0.0f, -1.0f, 0.0f };
xVec3 g_NZ3 = { 0.0f, 0.0f, -1.0f };

xVec3 g_Onez = { 1.0f, 1.0f, 1.0f };

xMat4x3 g_I3;

xQuat g_IQ = { 0.0f, 0.0f, 0.0f, 1.0f };

void xMath3Init()
{
    iMath3Init();

    g_I3.right.x = g_X3.x;
    g_I3.right.y = g_X3.y;
    g_I3.right.z = g_X3.z;

    g_I3.up.x = g_Y3.x;
    g_I3.up.y = g_Y3.y;
    g_I3.up.z = g_Y3.z;

    g_I3.at.x = g_Z3.x;
    g_I3.at.y = g_Z3.y;
    g_I3.at.z = g_Z3.z;

    g_I3.pos.x = g_O3.x;
    g_I3.pos.y = g_O3.y;
    g_I3.pos.z = g_O3.z;
}

void xMat3x3GetEuler(const xMat3x3 *m, xVec3 *a)
{
    float yaw;
    float pitch;
    float roll;

    pitch = -xasin(m->at.y);

    if (pitch < (PI / 2))
    {
        if (pitch > (-PI / 2))
        {
            yaw = xatan2(m->at.x, m->at.z);
            roll = xatan2(m->right.y, m->up.y);
        }
        else
        {
            yaw = -xatan2(-m->up.x, m->right.x);
            roll = 0.0f;
        }
    }
    else
    {
        yaw = xatan2(-m->up.x, m->right.x);
        roll = 0.0f;
    }

    a->x = yaw;
    a->y = pitch;
    a->z = roll;
}

void xMat4x3MoveLocalRight(xMat4x3 *m, float mag)
{
    m->pos.x += m->right.x * mag;
    m->pos.y += m->right.y * mag;
    m->pos.z += m->right.z * mag;
}

void xMat4x3MoveLocalUp(xMat4x3 *m, float mag)
{
    m->pos.x += m->up.x * mag;
    m->pos.y += m->up.y * mag;
    m->pos.z += m->up.z * mag;
}

void xMat4x3MoveLocalAt(xMat4x3 *m, float mag)
{
    m->pos.x += m->at.x * mag;
    m->pos.y += m->at.y * mag;
    m->pos.z += m->at.z * mag;
}

float xMat3x3LookVec(xMat3x3 *m, const xVec3 *at)
{
    float vec_len = xVec3Normalize(&m->at, at);

    xVec3Inv(&m->at, &m->at);

    if (CLOSEENOUGH2(1.0f - m->at.y, 0.0f))
    {
        m->right.x = 1.0f;
        m->right.y = 0.0f;
        m->right.z = 0.0f;

        m->up.x = 0.0f;
        m->up.y = 0.0f;
        m->up.z = 1.0f;

        m->at.x = 0.0f;
        m->at.y = -1.0f;
        m->at.z = 0.0f;

        return vec_len;
    }

    if (CLOSEENOUGH2(1.0f + m->at.y, 0.0f))
    {
        m->right.x = -1.0f;
        m->right.y = 0.0f;
        m->right.z = 0.0f;

        m->up.x = 0.0f;
        m->up.y = 0.0f;
        m->up.z = -1.0f;

        m->at.x = 0.0f;
        m->at.y = 1.0f;
        m->at.z = 0.0f;

        return vec_len;
    }

    if (CLOSEENOUGH2(at->z, 0.0f) && CLOSEENOUGH2(at->x, 0.0f))
    {
        m->right.x = 1.0f;
        m->right.y = 0.0f;
        m->right.z = 0.0f;

        m->up.x = 0.0f;
        m->up.y = 1.0f;
        m->up.z = 0.0f;

        m->at.x = 0.0f;
        m->at.y = 0.0f;
        m->at.z = 1.0f;

        return 0.0f;
    }

    m->right.x = m->at.z;
    m->right.y = 0.0f;
    m->right.z = -m->at.x;

    xVec3Normalize(&m->right, &m->right);
    xVec3Cross(&m->up, &m->at, &m->right);
    xVec3Cross(&m->right, &m->up, &m->at);

    m->flags = 0;

    return vec_len;
}

void xMat3x3Euler(xMat3x3 *m, const xVec3 *ypr)
{
    xMat3x3Euler(m, ypr->x, ypr->y, ypr->z);
}

void xMat3x3Euler(xMat3x3 *m, float yaw, float pitch, float roll)
{
    float f27 = isin(yaw);
    float f28 = icos(yaw);
    float f29 = isin(pitch);
    float f30 = icos(pitch);
    float f31 = isin(roll);
    float f1 = icos(roll);
    float f7 = f27 * f29;
    float f0 = f28 * f29;

    m->right.x = f28 * f1 + f31 * f7;
    m->right.y = f30 * f31;
    m->right.z = -f27 * f1 + f31 * f0;

    m->up.x = -f28 * f31 + f1 * f7;
    m->up.y = f30 * f1;
    m->up.z = f27 * f31 + f1 * f0;

    m->at.x = f27 * f30;
    m->at.y = -f29;
    m->at.z = f28 * f30;

    m->flags = 0;
}

void xQuatFromMat(xQuat *q, const xMat3x3 *m)
{
    const float *mp = (const float *)m;
    float *qvp = (float *)q;

    float tr = m->at.z + m->right.x + m->up.y;
    float root;

    if (tr > 0.0f)
    {
        root = xsqrt(1.0f + tr);

        q->s = 0.5f * root;
        q->v.x = 0.5f / root * (m->at.y - m->up.z);
        q->v.y = 0.5f / root * (m->right.z - m->at.x);
        q->v.z = 0.5f / root * (m->up.x - m->right.y);
    }
    else
    {
        int i = 0;
        int j;
        int k;

        if (m->up.y > m->right.x)
        {
            i = 1;
        }

        if (m->at.z > mp[i * 5])
        {
            i = 2;
        }

        static int nxt[3] = { 1, 2, 0 };

        j = nxt[i];
        k = nxt[j];

        root = xsqrt(1.0f + (mp[i * 5] - mp[j * 5] - mp[k * 5]));

        if (CLOSEENOUGH2(root, 0.0f))
        {
            xQuatCopy(q, &g_IQ);
        }
        else
        {
            qvp[i] = 0.5f * root;
            q->s = 0.5f / root * (mp[j + k * 4] - mp[k + j * 4]);
            qvp[j] = 0.5f / root * (mp[i + j * 4] + mp[j + i * 4]);
            qvp[k] = 0.5f / root * (mp[i + k * 4] + mp[k + i * 4]);

            if (q->s < 0.0f)
            {
                xQuatFlip(q, q);
            }
        }
    }
}

void xQuatToMat(const xQuat *q, xMat3x3 *m)
{
    float tx = 2.0f * q->v.x;
    float ty = 2.0f * q->v.y;
    float tz = 2.0f * q->v.z;

    float tsx = tx * q->s;
    float tsy = ty * q->s;
    float tsz = tz * q->s;

    float txx = tx * q->v.x;
    float txy = ty * q->v.x;
    float txz = tz * q->v.x;

    float tyy = ty * q->v.y;
    float tyz = tz * q->v.y;

    float tzz = tz * q->v.z;

    m->right.x = 1.0f - tyy - tzz;
    m->right.y = txy - tsz;
    m->right.z = txz + tsy;

    m->up.x = txy + tsz;
    m->up.y = 1.0f - tzz - txx;
    m->up.z = tyz - tsx;

    m->at.x = txz - tsy;
    m->at.y = tyz + tsx;
    m->at.z = 1.0f - txx - tyy;

    m->flags = 0;
}

void xQuatToAxisAngle(const xQuat *q, xVec3 *a, float *t)
{
    *t = 2.0f * xacos(q->s);
    xVec3Normalize(a, &q->v);
}

float xQuatNormalize(xQuat *o, const xQuat *q)
{
    float one_len = xQuatLength2(q);

    if (1.0f == one_len)
    {
        if (o != q)
        {
            xQuatCopy(o, q);
        }

        return 1.0f;
    }
    
    if (0.0f == one_len)
    {
        if (o != q)
        {
            xQuatCopy(o, &g_IQ);
        }

        return 0.0f;
    }

    one_len = xsqrt(one_len);

    xQuatSMul(o, q, 1.0f / one_len);

    return one_len;
}

void xQuatSlerp(xQuat *o, const xQuat *a, const xQuat *b, float t)
{
    float asph;
    float bsph;
    float one_sintheta;
    float abdot;
    xQuat b2;

    abdot = xQuatDot(a, b);

    if (abdot < 0.0f)
    {
        abdot = -abdot;

        b2.v.x = -b->v.x;
        b2.v.y = -b->v.y;
        b2.v.z = -b->v.z;
        b2.s = -b->s;

        b = &b2;
    }

    if (abdot >= 0.999f)
    {
        asph = t;
        bsph = 1.0f - t;
    }
    else
    {
        abdot = xacos(abdot);
        one_sintheta = 1.0f / isin(abdot);
        bsph = one_sintheta * isin((1.0f - t) * abdot);
        asph = one_sintheta * isin(t * abdot);
    }

    xQuat var_68;
    xQuat var_78;

    xQuatSMul(&var_68, a, bsph);
    xQuatSMul(&var_78, b, asph);
    xQuatAdd(o, &var_68, &var_78);
    xQuatNormalize(o, o);
}

void xQuatMul(xQuat *o, const xQuat *a, const xQuat *b)
{
    o->v.x = -(a->v.z * b->v.y - (a->v.y * b->v.z + (a->s * b->v.x + (a->v.x * b->s))));
    o->v.y = -(a->v.x * b->v.z - (a->v.z * b->v.x + (a->s * b->v.y + (a->v.y * b->s))));
    o->v.z = -(a->v.y * b->v.x - (a->v.x * b->v.y + (a->s * b->v.z + (a->v.z * b->s))));
    o->s = -(a->v.z * b->v.z - -(a->v.y * b->v.y - (a->s * b->s - (a->v.x * b->v.x))));

    xQuatNormalize(o, o);
}

void xQuatDiff(xQuat *o, const xQuat *a, const xQuat *b)
{
    xQuatConj(o, a);
    xQuatMul(o, o, b);

    if (o->s < 0.0f)
    {
        xQuatFlip(o, o);
    }
}
//#include "ArrayList.h"
//#include "../doom/p_local.h"
#include <time.h>
#include "DemoTrace.h"
#include "../i_system.h"
#include "../doom/r_state.h"
#include "SVG.h"

static void GenGray(double q, char val[7], boolean red)
{
    q *= 128;
    snprintf(val, 7, "%02x%02x%02x", red ? 255 : (int)q, (int)(q * !red), (int)(q * !red));
}

void SVG_WriteLevel(void)
{
    char name[81];
    snprintf(name, sizeof(name), "trace%llu.svg",
             (unsigned long long)time(NULL));
    FILE* f = fopen(name, "wt");
    if(!f)
        I_Error("Cannot open demotrace file for writing!");
    const line_t* l;
    const char* color;
    fixed_t minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
    for(int i = 0; i < numlines; ++i)
    {
        l = lines + i;
        if(l->v1->x < minx)
            minx = l->v1->x;
        if(l->v1->x > maxx)
            maxx = l->v1->x;
        if(l->v2->x < minx)
            minx = l->v2->x;
        if(l->v2->x > maxx)
            maxx = l->v2->x;

        if(l->v1->y < miny)
            miny = l->v1->y;
        if(l->v1->y > maxy)
            maxy = l->v1->y;
        if(l->v2->y < miny)
            miny = l->v2->y;
        if(l->v2->y > maxy)
            maxy = l->v2->y;
    }
    
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" "
          "xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"%d\" height=\"%d\">", (maxx - minx) / FRACUNIT, (maxy - miny) / FRACUNIT);
    
    for(int i = 0; i < numlines; ++i)
    {
        l = lines + i;
        fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
                "style=\"stroke:#", (l->v1->x - minx) / FRACUNIT,
                (l->v1->y - miny) / FRACUNIT,
                (l->v2->x - minx) / FRACUNIT,
                (l->v2->y - miny) / FRACUNIT);
        if(!l->backsector)
            color = "000000";
        else if(l->backsector->floorheight != l->frontsector->floorheight)
            color = "808080";
        else if(l->backsector->ceilingheight != l->frontsector->ceilingheight)
            color = "a0a0a0";
        else
            color = "c0c0c0";
        fprintf(f, "%s;\"/>", color);
    }
    
    // now write the player trace
    size_t j;
    const PlayerPos* pp;
    char dcolor[7];
    size_t n;
    for(int i = 0; i < MAXPLAYERS; ++i)
    {
        n = dt_playerPos[i].nelts;
        if(n <= 1)
            continue;
        
        pp = dt_playerPos[i].elts;
        for(j = 1; j < n; ++j)
        {
            GenGray((double)j / n, dcolor, pp[j].armed);
            fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
                    "style=\"stroke:#%s;\"/>",
                    (pp[j - 1].x - minx) / FRACUNIT,
                    (pp[j - 1].y - miny) / FRACUNIT,
                    (pp[j].x - minx) / FRACUNIT,
                    (pp[j].y - miny) / FRACUNIT, dcolor);
        }
    }
    
    n = dt_itemPos.nelts;
    pp = dt_itemPos.elts;
    for(int i = 0; i < n; ++i)
    {
        fprintf(f, "<circle cx=\"%d\" cy=\"%d\" r=\"4\" fill=\"blue\"/>",
                (pp[i].x - minx) / FRACUNIT, (pp[i].y - miny) / FRACUNIT);
    }
    
    fputs("</svg>", f);
    if(fclose(f) < 0)
        I_Error("Cannot close demotrace file!");
}
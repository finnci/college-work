//
//  objparser.h
//  lab02
//
//  Created by Ciaran F on 03/11/2014.
//  Copyright (c) 2014 Ciaran F. All rights reserved.
//

#ifndef lab02_objparser_h
#define lab02_objparser_h


bool load_obj_file (
                    const char* file_name,
                    float*& points,
                    float*& tex_coords,
                    float*& normals,
                    int& point_count
                    );

#endif

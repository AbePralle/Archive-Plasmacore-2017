//
//  Shader.fsh
//  PlasmacoreiOS
//
//  Created by Abe Pralle on 4/13/15.
//  Copyright (c) 2015 Abe Pralle. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}

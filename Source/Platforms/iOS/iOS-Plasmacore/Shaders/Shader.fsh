//
//  Shader.fsh
//  iOS-Plasmacore
//
//  Created by Abraham Pralle on 5/11/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}

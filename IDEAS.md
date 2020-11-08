poiint_cloud
    - vertices
        -position
        -uniform color
        -uniform size
        -varying color
        -varying size
        -vectorfield
        -splat
            -x_direction
            -y_direction

vectorfield
    -vertices
        -position
        -direction
        -uniform color
        -uniform size
        -varying color
        -varying size

graph : point_cloud
    -edges
        -position
        -uniform color
        -varying color
        -vectorfield

mesh : graph
    -faces
        -position
        -uniform color
        -varying color
        -vectorfield
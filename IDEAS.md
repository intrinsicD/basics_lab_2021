Rendering programs:

points
- vertices
  - varying position
  - varying color
  - varying size
  - use_uniform_color
  - use_uniform_size
  - uniform color
  - uniform size
  - uniform alpha

splats
- vertices
  - varying position
  - varying color
  - varying size
  - varying x_direction
  - varying y_direction
  - use_uniform_color
  - use_uniform_size
  - uniform color
  - uniform size
  - uniform alpha

vectorfield
- vertices
  - varying position
  - varying color
  - varying size
  - varying direction
  - use_uniform_color
  - use_uniform_size
  - uniform color
  - uniform size
  - uniform alpha

graph : point_cloud
- edges
  - varying position
  - varying color
  - use_uniform_color
  - uniform color
  - uniform alpha

mesh : graph
- faces
  - varying position
  - varying color
  - use_uniform_color
  - uniform color
  - uniform alpha


TODO:
    Gaussian Product Subdivision Surfaces:
        1. Covmesh generation:
            - Vertex based Covariance estimation
            - Face based Covariance estimation
            - Gaussian Mixture Model construction
            - Hierarchical Gaussian Mixture Model construction
        2. Rendering of covmesh limit surface:
            - Renderer for covmeshed:
                - pass inverse cov-matrices from vertex shader to fragment shader
                - use formula for limit mean to set framgents position
                - then apply transformation matrices
        3. Compute Normals of covmesh from formula
        4. Compute curvatures of covmesh from formula
        5. Reconstruction using Greedy triangulation method
        6. Investigate other linear operator applied in dual - hypermesh domain to obtain nonlinear operators in primal - mesh domain  
            - Laplacian?  
            - Resampling?

viewer:
    scene:
        list of entities;
    main:
        initialize;
        main loop:
            query events;
            update scene:
                for each update event:
                    process update event;
            render scene:
                determine visible entities;
                for each visible entity:
                    enqueue render event with material and transform;

                for each renderer:
                    process render event;


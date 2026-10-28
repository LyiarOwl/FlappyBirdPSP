#pragma once

typedef enum layers_mask {
    LAYER_MASK_NONE   = 0,
    LAYER_MASK_BIRD   = 1 << 0,
    LAYER_MASK_PIPE   = 1 << 1,
    LAYER_MASK_GROUND = 1 << 2,
    LAYER_MASK_SCORE  = 1 << 3
} Layers_Mask;

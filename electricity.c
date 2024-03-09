/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2022 Beaver (GEGL Electricity)
 */

/*
Graph of GEGL electricity to test without installing - like all Graphs it may not be 100% accurate.

id=1  src-atop aux=[ ref=1  plasma width=1280 height=1280  seed=7 exposure exposure=-0.3 unsharp-mask scale=1   ]
cubism tile-size=23 tile-saturation=99 seed=4 mean-curvature-blur iterations=27  oilify mask-radius=14 edge color-to-alpha color=#000000 gimp:desaturate  mode=value 
id=2 dst-over aux=[ ref=2  gaussian-blur std-dev-x=3 std-dev-y=3]
 */


#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (tile_size, _("Internal Cubism Size"), 19.0)
    description (_("Average diameter of each tile (in pixels)"))
    value_range (6.0, 35.0)
    ui_meta     ("unit", "pixel-distance")

property_double (tile_saturation, _("Internal Cubism Spacing"), 4.0)
    description (_("Expand tiles by this amount"))
    value_range (2, 9.0)

property_seed (seed, _("Internal cubism seed randomizer"), rand)
  description (_("The random seed for electricity"))


property_double (transparency_threshold, _("Reduce Electricity effect"), 0.064)
    description(_("Threshold Alpha helps reduce the electricity effect"))
    value_range (0.020, 0.600)



property_int    (oil, _("Internal Oilify Mask Radius"), 14)
    description (_("Oilify to make things look -more electric-"))
    value_range (8, 30)
    ui_range (8, 30)
    ui_meta     ("unit", "pixel-distance")

property_int  (iterations, _("Internal Mean Curvature"), 20)
  description (_("Mean Curvature Blur to Smooth things"))
  value_range (0, 30)
  ui_range    (0, 30)

property_double (opacity, _("Hyper Opacity"), 1.0)
    description (_("Make the opacity meter go to to 200%"))
    value_range (1, 2)
    ui_range    (1, 2)


property_int (blur, _("Blur rough edges if they exist"), 1)
   description(_("A Box Blur looks better then gaussian for this case"))
   value_range (0, 1)
   ui_range    (0, 1)
   ui_gamma   (1.5)


property_color (col, _("Color of Electricity"), "#ffffff")





#else

#define GEGL_OP_META
#define GEGL_OP_NAME     electricity
#define GEGL_OP_C_SOURCE electricity.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *c2a, *col, *oil, *cubism, *alock, *mc, *blur, *edge, *opacity, *plasma;
  GeglColor *hidden_color_electricity = gegl_color_new ("#000000");

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");


  c2a = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-to-alpha", "color", hidden_color_electricity, NULL);
                           

  opacity = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);


  col = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",     NULL);
                     

  oil = gegl_node_new_child (gegl,
                                  "operation", "gegl:oilify",
                                  NULL);

  blur = gegl_node_new_child (gegl,
                                  "operation", "gegl:box-blur",
                                  NULL);

  alock = gegl_node_new_child (gegl,
                                  "operation", "gegl:src-atop",
                                  NULL);

  edge = gegl_node_new_child (gegl,
                                  "operation", "gegl:edge",
                                  NULL);

  mc = gegl_node_new_child (gegl,
                                  "operation", "gegl:mean-curvature-blur",
                                  NULL);

  cubism = gegl_node_new_child (gegl,
                                  "operation", "gegl:cubism",
                                  NULL);

  plasma = gegl_node_new_child (gegl,
                                  "operation", "gegl:plasma",
                                  NULL);

  gegl_operation_meta_redirect (operation, "transparency_threshold", c2a, "transparency-threshold");
  gegl_operation_meta_redirect (operation, "seed", cubism, "seed");
  gegl_operation_meta_redirect (operation, "tile_saturation", cubism, "tile-saturation");
  gegl_operation_meta_redirect (operation, "tile_size", cubism, "tile-size");
  gegl_operation_meta_redirect (operation, "iterations", mc, "iterations");
  gegl_operation_meta_redirect (operation, "oil", oil, "mask-radius");
  gegl_operation_meta_redirect (operation, "col", col, "value");
  gegl_operation_meta_redirect (operation, "opacity", opacity, "value");
  gegl_operation_meta_redirect (operation, "blur", blur, "radius");



  gegl_node_link_many (input, alock, cubism, mc, oil, edge, c2a, blur, col, opacity, output, NULL);
  gegl_node_connect (alock, "aux", plasma, "output");




}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:electricity",
    "title",       _("Electricity over Alpha"),
    "categories",  "Aristic",
    "reference-hash", "2vedb6g36a28f25j0vvf05sb2ac",
    "description", _("Electric Effect with GEGL. Blend it with Gimp's Linear Light blend mode on mid opacity for cool results. "
                     ""),
    NULL);
}

#endif

/*
 * This file is part of the libopentracedecode project.
 *
 * Copyright (C) 2012 Bert Vermeulen <bert@biot.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include "libopentracedecode-internal.h" /* First, so we avoid a _POSIX_C_SOURCE warning. */
#include <opentracedecode/libopentracedecode.h>

/** @cond PRIVATE */

/*
 * When initialized, a reference to this module inside the Python interpreter
 * lives here.
 */
OTD_PRIV PyObject *mod_sigrokdecode = NULL;

/** @endcond */

static struct PyModuleDef sigrokdecode_module = {
	PyModuleDef_HEAD_INIT,
	.m_name = "sigrokdecode",
	.m_doc = "sigrokdecode module",
	.m_size = -1,
};

/** @cond PRIVATE */
PyMODINIT_FUNC PyInit_opentracedecode(void)
{
	PyObject *mod, *Decoder_type;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();

	mod = PyModule_Create(&sigrokdecode_module);
	if (!mod)
		goto err_out;

	Decoder_type = otd_Decoder_type_new();
	if (!Decoder_type)
		goto err_out;
	if (PyModule_AddObject(mod, "Decoder", Decoder_type) < 0)
		goto err_out;

	/* Expose output types as symbols in the sigrokdecode module */
	if (PyModule_AddIntConstant(mod, "OUTPUT_ANN", OTD_OUTPUT_ANN) < 0)
		goto err_out;
	if (PyModule_AddIntConstant(mod, "OUTPUT_PYTHON", OTD_OUTPUT_PYTHON) < 0)
		goto err_out;
	if (PyModule_AddIntConstant(mod, "OUTPUT_BINARY", OTD_OUTPUT_BINARY) < 0)
		goto err_out;
	if (PyModule_AddIntConstant(mod, "OUTPUT_LOGIC", OTD_OUTPUT_LOGIC) < 0)
		goto err_out;
	if (PyModule_AddIntConstant(mod, "OUTPUT_META", OTD_OUTPUT_META) < 0)
		goto err_out;
	/* Expose meta input symbols. */
	if (PyModule_AddIntConstant(mod, "OTD_CONF_SAMPLERATE", OTD_CONF_SAMPLERATE) < 0)
		goto err_out;

	mod_sigrokdecode = mod;

	PyGILState_Release(gstate);

	return mod;

err_out:
	Py_XDECREF(mod);
	otd_exception_catch("Failed to initialize module");
	PyGILState_Release(gstate);

	return NULL;
}

/** @endcond */

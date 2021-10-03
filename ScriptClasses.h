/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef SCRIPTCLASSES_H
#define SCRIPTCLASSES_H

static duk_idx_t duk_push_fvector(duk_context* ctx, FVector vector);

//new FVector(0, 0, 0);
//new FVector([0, 0, 0]);
static duk_ret_t duk_fvector(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	FVector Vector = { 0, 0, 0 };

	if (duk_is_array(ctx, 0)) {
		ARRAY_LENGTH(ctx, 0, 3);

		duk_get_prop_index(ctx, 0, 0);
		Vector.X = (float)duk_get_number(ctx, -1);

		duk_get_prop_index(ctx, 0, 1);
		Vector.Y = (float)duk_get_number(ctx, -1);

		duk_get_prop_index(ctx, 0, 2);
		Vector.Z = (float)duk_get_number(ctx, -1);
	}
	else if (duk_get_top(ctx) == 3) {
		Vector.X = (float)duk_get_number(ctx, 0);
		Vector.Y = (float)duk_get_number(ctx, 1);
		Vector.Z = (float)duk_get_number(ctx, 2);
	}
	else {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "Invalid arguments.");
		return duk_throw(ctx);
	}

	duk_push_fvector(ctx, Vector);

	return 1;
}

static bool duk_is_fvector(duk_context* ctx, duk_idx_t idx)
{
	return duk_is_object(ctx, idx) && duk_has_prop_string(ctx, idx, "X") && duk_has_prop_string(ctx, idx, "Y") && duk_has_prop_string(ctx, idx, "Z");
}

static FVector duk_get_fvector(duk_context* ctx, duk_idx_t idx)
{
	if (duk_is_fvector(ctx, idx)) {
		duk_get_prop_string(ctx, idx, "X");
		float x = (float)duk_get_number(ctx, -1);

		duk_get_prop_string(ctx, idx, "Y");
		float y = (float)duk_get_number(ctx, -1);

		duk_get_prop_string(ctx, idx, "Z");
		float z = (float)duk_get_number(ctx, -1);
		return FVector{ x, y, z };
	}
	else if (duk_is_array(ctx, idx)) {
		duk_get_prop_index(ctx, idx, 0);
		float x = (float)duk_get_number(ctx, -1);

		duk_get_prop_index(ctx, idx, 1);
		float y = (float)duk_get_number(ctx, -1);

		duk_get_prop_index(ctx, idx, 2);
		float z = (float)duk_get_number(ctx, -1);
		return FVector{ x, y, z };
	}
}

static duk_idx_t duk_push_fvector(duk_context* ctx, FVector vector)
{
	auto obj = duk_push_object(ctx);

	duk_push_number(ctx, vector.X);
	duk_put_prop_string(ctx, obj, "X");

	duk_push_number(ctx, vector.Y);
	duk_put_prop_string(ctx, obj, "Y");

	duk_push_number(ctx, vector.Z);
	duk_put_prop_string(ctx, obj, "Z");

	return obj;
}

static duk_idx_t duk_push_frotator(duk_context* ctx, FRotator rotator);

//new FRotator(0, 0, 0);
//new FRotator([0, 0, 0]);
static duk_ret_t duk_frotator(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	FRotator rotator = { 0, 0, 0 };

	if (duk_is_array(ctx, 0)) {
		ARRAY_LENGTH(ctx, 0, 3);

		duk_get_prop_index(ctx, 0, 0);
		rotator.Pitch = (float)duk_get_number(ctx, -1);

		duk_get_prop_index(ctx, 0, 1);
		rotator.Yaw = (float)duk_get_number(ctx, -1);

		duk_get_prop_index(ctx, 0, 2);
		rotator.Roll = (float)duk_get_number(ctx, -1);
	}
	else if (duk_get_top(ctx) == 3) {
		rotator.Pitch = (float)duk_get_number(ctx, 0);
		rotator.Yaw = (float)duk_get_number(ctx, 1);
		rotator.Roll = (float)duk_get_number(ctx, 2);
	}
	else {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "Invalid arguments.");
		return duk_throw(ctx);
	}

	duk_push_frotator(ctx, rotator);

	return 1;
}

static bool duk_is_frotator(duk_context* ctx, duk_idx_t idx)
{
	return duk_is_object(ctx, idx) && duk_has_prop_string(ctx, idx, "Pitch") && duk_has_prop_string(ctx, idx, "Yaw") && duk_has_prop_string(ctx, idx, "Roll");
}

static FRotator duk_get_frotator(duk_context* ctx, duk_idx_t idx)
{
	if (duk_is_frotator(ctx, idx)) {
		duk_get_prop_string(ctx, idx, "Pitch");
		float pitch = (float)duk_get_number(ctx, -1);

		duk_get_prop_string(ctx, idx, "Yaw");
		float yaw = (float)duk_get_number(ctx, -1);

		duk_get_prop_string(ctx, idx, "Roll");
		float roll = (float)duk_get_number(ctx, -1);

		return FRotator{ pitch, yaw, roll };
	}
	else if (duk_is_array(ctx, idx)) {
		duk_get_prop_index(ctx, idx, 0);
		float pitch = (float)duk_get_number(ctx, -1);

		duk_get_prop_index(ctx, idx, 1);
		float yaw = (float)duk_get_number(ctx, -1);

		duk_get_prop_index(ctx, idx, 2);
		float roll = (float)duk_get_number(ctx, -1);
		return FRotator{ pitch, yaw, roll };
	}
}

static duk_idx_t duk_push_frotator(duk_context* ctx, FRotator rotator)
{
	auto obj = duk_push_object(ctx);

	duk_push_number(ctx, rotator.Pitch);
	duk_put_prop_string(ctx, obj, "Pitch");

	duk_push_number(ctx, rotator.Yaw);
	duk_put_prop_string(ctx, obj, "Yaw");

	duk_push_number(ctx, rotator.Roll);
	duk_put_prop_string(ctx, obj, "Roll");

	return obj;
}

enum class TArrayType {
	Pointer,
	Integer
};

// void TArray.clear()
static duk_idx_t duk_tarray_clear(duk_context* ctx)
{
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "ptr");
	auto ptr = duk_get_pointer(ctx, -1);

	auto arr = (TArray<void*>*)ptr;
	
	arr->Clear();

	return 0;
}

// T TArray.get(0)
static duk_idx_t duk_tarray_get(duk_context* ctx)
{
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "ptr");
	auto ptr = duk_get_pointer(ctx, -1);

	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "type");
	auto type = (TArrayType)duk_get_int(ctx, -1);

	auto arr = (TArray<void*>*)ptr;

	// probably not the best solution
	if (duk_is_number(ctx, 0)) {
		auto idx = duk_get_int(ctx, 0);

		if (type == TArrayType::Pointer) {
			duk_push_pointer(ctx, arr->operator[](idx));
		}
		else if (type == TArrayType::Integer) {
			duk_push_int(ctx, ((TArray<int>*)ptr)->operator[](idx));
		}
		else {
			duk_push_null(ctx);
		}
	}
	else {
		auto arrIdx = duk_push_array(ctx);

		for (int i = 0; i < arr->Num(); i++) {
			if (type == TArrayType::Pointer) {
				duk_push_pointer(ctx, arr->operator[](i));
			}
			else if (type == TArrayType::Integer) {
				duk_push_int(ctx, ((TArray<int>*)ptr)->operator[](i));
			}
			else {
				duk_push_null(ctx);
			}
			duk_put_prop_index(ctx, arrIdx, i);
		}
	}

	return 1;
}

// int TArray.length
static duk_idx_t duk_tarray_length(duk_context* ctx)
{
	duk_push_this(ctx);

	duk_get_prop_string(ctx, -1, "ptr");
	auto arr = (TArray<void*>*)duk_get_pointer(ctx, -1);

	duk_push_int(ctx, arr->Num());

	return 1;
}

template <class T>
static duk_idx_t duk_push_tarray(duk_context* ctx, TArray<T>* tarray)
{
	auto obj = duk_push_object(ctx);

	duk_push_pointer(ctx, tarray);
	duk_put_prop_string(ctx, obj, "ptr");

	if (typeid(T) == typeid(int)) {
		duk_push_int(ctx, (int)TArrayType::Integer);
	}
	else {
		duk_push_int(ctx, (int)TArrayType::Pointer);
	}
	duk_put_prop_string(ctx, obj, "type");

	duk_push_c_function(ctx, duk_tarray_get, DUK_VARARGS);
	duk_put_prop_string(ctx, obj, "get");

	duk_push_c_function(ctx, duk_tarray_clear, DUK_VARARGS);
	duk_put_prop_string(ctx, obj, "clear");

	// modifying tarrays is planned

	duk_push_string(ctx, "length");
	duk_push_c_function(ctx, duk_tarray_length, DUK_VARARGS);
	duk_def_prop(ctx, obj, DUK_DEFPROP_HAVE_GETTER);

	return obj;
}

#endif // SCRIPTCLASSES_H
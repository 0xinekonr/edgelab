package com.edgelab.backend.common.error;

import java.util.List;

public record ApiErrorResponse(
        String code,
        String message,
        List<FieldErrorResponse> fieldErrors
) {
}

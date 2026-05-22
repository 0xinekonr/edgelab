package com.edgelab.backend.telemetry;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;

import java.math.BigDecimal;
import java.time.Instant;

public record TelemetryRequest(
        @NotBlank String deviceId,
        @NotBlank String metricCode,
        @NotNull BigDecimal value,
        @NotBlank String unit,
        @NotNull Instant collectedAt
) {
}

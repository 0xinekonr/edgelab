package com.edgelab.backend.telemetry;

public record TelemetryResponse(
        String telemetryId,
        String deviceId,
        String status
) {
}

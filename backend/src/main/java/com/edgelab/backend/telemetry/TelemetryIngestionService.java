package com.edgelab.backend.telemetry;

import org.springframework.stereotype.Service;

import java.util.UUID;

@Service
public class TelemetryIngestionService {

    public TelemetryResponse ingest(TelemetryRequest request) {
        return new TelemetryResponse(
                UUID.randomUUID().toString(),
                request.deviceId(),
                "ACCEPTED"
        );
    }
}

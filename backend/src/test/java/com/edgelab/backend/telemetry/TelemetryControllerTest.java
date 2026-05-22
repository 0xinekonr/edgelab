package com.edgelab.backend.telemetry;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.http.MediaType;
import org.springframework.test.web.servlet.MockMvc;

import static org.hamcrest.Matchers.not;
import static org.hamcrest.Matchers.blankOrNullString;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;
import static org.springframework.test.web.servlet.result.MockMvcResultHandlers.print;

@SpringBootTest
@AutoConfigureMockMvc
public class TelemetryControllerTest {

    @Autowired
    private MockMvc mockMvc;

    @Test
    void shouldAcceptValidTelemetryReading() throws Exception {
        String requestBody = """
                {
                    "deviceId": "pump-001",
                    "metricCode": "temperature",
                    "value": "72.5",
                    "unit": "celsius",
                    "collectedAt": "2026-05-20T10:30:00Z"
                }
                """;

        mockMvc.perform(post("/api/v1/telemetry")
                        .contentType(MediaType.APPLICATION_JSON)
                        .content(requestBody))
                .andDo(print())
                .andExpect(status().isAccepted())
                .andExpect(jsonPath("$.telemetryId", not(blankOrNullString())))
                .andExpect(jsonPath("$.deviceId").value("pump-001"))
                .andExpect(jsonPath("$.status").value("ACCEPTED"));
    }

    @Test
    void shouldReturnValidationErrorWhenDeviceIdIsBlank() throws Exception {
        String requestBody = """
                {
                  "deviceId": "",
                  "metricCode": "temperature",
                  "value": 72.5,
                  "unit": "celsius",
                  "collectedAt": "2026-05-20T10:30:00Z"
                }
                """;

        mockMvc.perform(post("/api/v1/telemetry")
                        .contentType(MediaType.APPLICATION_JSON)
                        .content(requestBody))
                .andExpect(status().isBadRequest())
                .andExpect(jsonPath("$.code").value("VALIDATION_FAILED"))
                .andExpect(jsonPath("$.message").value("Request validation failed"))
                .andExpect(jsonPath("$.fieldErrors[0].field").value("deviceId"))
                .andExpect(jsonPath("$.fieldErrors[0].message").value("must not be blank"));
    }
}

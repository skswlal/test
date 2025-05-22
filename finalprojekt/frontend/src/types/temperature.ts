export interface TemperatureReading {
  id: number;
  timestamp: string;
  temperature: number;
}

export interface TemperatureData {
  readings: TemperatureReading[];
  isLoading: boolean;
  error: string | null;
} 
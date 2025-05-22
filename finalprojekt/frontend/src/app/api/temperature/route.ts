import { NextResponse } from 'next/server';
import { getTemperatureReadings, addTemperatureReading } from '@/lib/db';

// GET handler
export async function GET() {
  try {
    const readings = await getTemperatureReadings();
    return NextResponse.json(readings);
  } catch (error) {
    console.error('Error fetching temperatures:', error);
    return NextResponse.json(
      { error: 'Internal server error' },
      { status: 500 }
    );
  }
}

// POST handler
export async function POST(request: Request) {
  try {
    const { temperature } = await request.json();
    const reading = await addTemperatureReading(temperature);
    return NextResponse.json(reading);
  } catch (error) {
    console.error('Error saving temperature:', error);
    return NextResponse.json(
      { error: 'Internal server error' },
      { status: 500 }
    );
  }
} 
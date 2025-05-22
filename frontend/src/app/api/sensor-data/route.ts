import { NextResponse } from 'next/server';

export async function GET() {
  try {
    const response = await fetch('http://localhost:3001/api/sensor-data');
    const data = await response.json();
    
    return NextResponse.json(data);
  } catch (error) {
    console.error('Error fetching sensor data:', error);
    return NextResponse.json(
      { error: 'Failed to fetch sensor data' },
      { status: 500 }
    );
  }
} 

export async function POST(request: Request) {
  try {
    const body = await request.json();
    // You can log or process the incoming sensor data here
    console.log('Received sensor data:', body);

    // Optionally, forward it to your backend or database here

    return NextResponse.json({ message: 'Data received', data: body }, { status: 200 });
  } catch (error) {
    console.error('Error receiving sensor data:', error);
    return NextResponse.json(
      { error: 'Failed to receive sensor data' },
      { status: 500 }
    );
  }
}